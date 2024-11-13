#include "DigSpot.h"

#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Gp27_TTC/Base/MovingCamera.h"
#include "Kismet/GameplayStatics.h"

ADigSpot::ADigSpot()
{
	PrimaryActorTick.bCanEverTick = true;

	m_InteractableType = InteractableType::HELD;
	m_bIsInteractable = false;
	m_bDoWaterSplash = false;
}

void ADigSpot::BeginPlay()
{
	Super::BeginPlay();

	m_pWidgetComponent = GetComponentByClass<UWidgetComponent>();
	m_pWidgetComponent->SetVisibility(false);
	m_pCameraComponent = UGameplayStatics::GetActorOfClass(GetWorld(), AMovingCamera::StaticClass())->FindComponentByClass<UCameraComponent>();
	m_pPickupRadiusSphereComponent = FindComponentByClass<USphereComponent>();
	m_pDecal = FindComponentByClass<UDecalComponent>();

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("DecomposeUpdate"));
	FOnTimelineEvent DecomposeFinished;
	DecomposeFinished.BindUFunction(this, FName("DecomposeFinished"));

	m_DecompositionTimeLine.AddInterpFloat(m_pDecompositionCurve, ProgressUpdate);
	m_DecompositionTimeLine.SetTimelineFinishedFunc(DecomposeFinished);

	m_pMaterial = m_pDecal->GetMaterial(0);
	m_pDynamicMaterial = UMaterialInstanceDynamic::Create(m_pMaterial, m_pDecal);
	m_pDecal->SetMaterial(0, m_pDynamicMaterial);
}

void ADigSpot::RotateInteractionPrompt()
{
	// Get the positions of the decal and the camera
	const FVector decalPosition = m_pWidgetComponent->GetComponentLocation();
	const FVector cameraPosition = m_pCameraComponent->GetComponentLocation();

	// Calculate direction from the decal to the camera
	const FVector direction = (cameraPosition - decalPosition).GetSafeNormal();

	// Create a rotation that faces the direction, ignoring Z rotation
	FRotator LookAtRotation = FRotationMatrix::MakeFromX(direction).Rotator();
	LookAtRotation.Roll = 0.0f; // Zero out the roll to prevent Z-axis rotation
	
	// Apply the rotation to the decal component
	m_pWidgetComponent->SetWorldRotation(LookAtRotation);
}

void ADigSpot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_bInteractionStarted)
		m_CurrentDigTime += DeltaTime;
	
	if (m_CurrentDigTime >= m_TimeToDig && !m_bHasSpawnedTreasure)
	{
		const FVector location = GetActorLocation();
		m_bHasSpawnedTreasure = true;

		OnInteractionCompleted.Broadcast();
		GetWorld()->SpawnActor(m_pTreasureChestClass, &location);
		m_bDecomposing = true;
		m_DecompositionTimeLine.Play();
	}

	RotateInteractionPrompt();
	
	if (m_pPickupRadiusSphereComponent->IsOverlappingActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) ||
		m_pPickupRadiusSphereComponent->IsOverlappingActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 1)) ||
		m_pPickupRadiusSphereComponent->IsOverlappingActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 2)) ||
		m_pPickupRadiusSphereComponent->IsOverlappingActor(UGameplayStatics::GetPlayerCharacter(GetWorld(), 3)))
	{
		m_pWidgetComponent->SetVisibility(true);
		m_bIsInteractable = true;
	}
	else
	{
		m_bIsInteractable = false;
		m_pWidgetComponent->SetVisibility(false);
	}
	if(m_bDecomposing)
	{		
		m_DecompositionTimeLine.TickTimeline(DeltaTime);		
	}
}

void ADigSpot::OnDeletion()
{
	IWaterInteractable::OnDeletion();

	OnInteractionCompleted.Broadcast();
}

void ADigSpot::Interact()
{
	Super::Interact();

	if (!m_bIsInteractable) return;
	
	if (!m_bInteractionStarted)
	{
		m_bInteractionStarted = true;
		OnInteractionStarted.Broadcast(this); //Only broadcast it once
	}
}

void ADigSpot::InteractStop()
{
	Super::InteractStop();

	m_bInteractionStarted = false;
}
void ADigSpot::SetIsDecomposing(bool isDecomposing)
{
	m_bDecomposing = isDecomposing;
	m_DecompositionTimeLine.Play();
}

void ADigSpot::DecomposeUpdate(float DeltaTime)
{
	m_Decomposition = m_pDecompositionCurve->GetFloatValue(m_DecompositionTimeLine.GetPlaybackPosition());
	m_pDynamicMaterial->SetScalarParameterValue(TEXT("Burn Amount"), m_Decomposition);
}

void ADigSpot::DecomposeFinished()
{	
	Destroy();
}

