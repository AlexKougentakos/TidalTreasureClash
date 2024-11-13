#include "Cannon.h"

#include "CannonBall.h"
#include "CannonIndicator.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Character.h"
#include "Gp27_TTC/Base/MovingCamera.h"
#include "Gp27_TTC/Events/MeshPointGetter.h"
#include "Kismet/GameplayStatics.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Gp27_TTC/Character/MainPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACannon::ACannon()
{
	PrimaryActorTick.bCanEverTick = true;

	m_InteractableType = InteractableType::PRESSED;
	m_bIsInteractable = false;
	
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	m_pWidgetComponent = GetComponentByClass<UWidgetComponent>();
	m_pWidgetComponent->SetVisibility(false);
	m_pCameraComponent = UGameplayStatics::GetActorOfClass(GetWorld(), AMovingCamera::StaticClass())->FindComponentByClass<UCameraComponent>();
	m_pPickupRadiusSphereComponent = FindComponentByClass<USphereComponent>();

	checkf(m_pIndicator, TEXT("no indicator references set"));
	m_pIndicator->SetVisibility(false);	
	m_pFuseNiagaraSystem = FindComponentByTag<UNiagaraComponent>(FName{"Fuse"});
	m_pFuseNiagaraSystem->SetVisibility(false);
	
	TArray<AActor*> pointGetters{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMeshPointGetter::StaticClass(), pointGetters);
	m_pPointGetter = Cast<AMeshPointGetter>(pointGetters[0]);
	m_TargetLimitX = m_pPointGetter->GetActorLocation().X + 800;
	m_TargetLimitY = m_pPointGetter->GetActorLocation().Y + 850;

	m_pBaseCannon = FindComponentByTag<UStaticMeshComponent>(FName{"CannonBase"});
	m_pBarrelCannon = FindComponentByTag<UStaticMeshComponent>(FName{"CannonMain"});
}

void ACannon::RotateInteractionPrompt()
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

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!m_bReadyToFire)
	{
		m_ShootingTimer += DeltaTime;
	}

	if(m_ShootingTimer >= m_ShootingDelay)
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_pCannonReloadSound, GetActorLocation());
		m_bReadyToFire = true;
		m_ShootingTimer = 0.f;
		m_pFuseNiagaraSystem->SetVisibility(true);
	}
	
	if(m_bUpdateTarget)
	{
		FVector desiredDirection{m_DesiredDirection.X, -m_DesiredDirection.Y, 0};
		FVector desiredLocation = m_pIndicator->GetCapsuleLocation() + desiredDirection * m_TargetSpeed * DeltaTime;
		if(desiredLocation.X > m_TargetLimitX) desiredLocation.X = m_TargetLimitX;
		if(desiredLocation.X < -m_TargetLimitX) desiredLocation.X = -m_TargetLimitX;
		if(desiredLocation.Y > m_TargetLimitY) desiredLocation.Y = m_TargetLimitY;
		if(desiredLocation.Y < -m_TargetLimitY) desiredLocation.Y = -m_TargetLimitY;		
		//todo: cannon indicator ref -> set world location

		m_pIndicator->UpdatePosition(desiredLocation);		
		m_pBaseCannon->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FVector{desiredLocation.X, desiredLocation.Y, GetActorLocation().Z}));

		m_bUpdateTarget = false;
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
}

void ACannon::SetCharacter(AMainCharacter* characterRef)
{
	m_pInteractingCharacter = characterRef;
	m_pIndicator->SetVisibility(true);
}

void ACannon::ResetCharachterRef()
{
	m_pInteractingCharacter = nullptr;
	m_pIndicator->SetVisibility(false);
}

void ACannon::Interact()
{
	Super::Interact();	
	GEngine->AddOnScreenDebugMessage(20, -1, FColor::Orange, "cannon interacted");
	AMainPlayerController* controller = Cast<AMainPlayerController>(m_pInteractingCharacter->GetController());
	controller->SetInputToCannon();
}

void ACannon::MoveTarget(FVector2D targetMovement)
{
	m_bUpdateTarget = true;
	m_DesiredDirection = targetMovement;
}

void ACannon::Fire()
{
	if(!m_bReadyToFire)
	{
		return;
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = m_pInteractingCharacter;

	const FVector startPosition = m_pBarrelCannon->GetSocketLocation(FName{"LaunchSocket"});
	const FVector targetPosition = m_pIndicator->GetTargetLocation();
	const FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(m_pBarrelCannon->GetSocketLocation(FName{"LaunchSocket"}), targetPosition);
	const ACannonBall* CannonBall = Cast<ACannonBall>(GetWorld()->SpawnActor<AActor>(m_CannonBallClass,startPosition, FRotator{0, newRotation.Yaw, 0}, SpawnParams));

	const float gravity = GetWorld()->GetGravityZ() * -1;
	constexpr float theta = (30 * PI / 180); //maybe add field or get angle from cannon
	FVector dir = targetPosition - startPosition; //direction
	const float Sz = dir.Z; // Height difference
	dir.Z = 0; // Remove height from direction
	const float Sx = dir.Size(); // Distance

	const float V = (Sx / cos(theta)) * FMath::Sqrt((gravity * 1) / (2 * (Sx * tan(theta) - Sz)));
	const FVector VelocityOutput = FVector(V*cos(theta), 0, V*sin(theta));
			
	CannonBall->m_pProjectileMovement->SetVelocityInLocalSpace(VelocityOutput);
	UGameplayStatics::PlaySound2D(GetWorld(), m_pCannonShotSound, 1);
	UNiagaraComponent* niagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		 GetWorld(),
		 m_pCannonShotNiagaraSystem,
		 startPosition,
		 FRotator(newRotation),
		 FVector(1)
		 );

	m_bReadyToFire = false;
	m_pFuseNiagaraSystem->SetVisibility(false);

	//Apply camera shake
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientStartCameraShake(m_pCameraShake);
}

