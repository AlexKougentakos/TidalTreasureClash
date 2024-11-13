// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Gp27_TTC/ImGuiDebugger.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh Component"));
	m_pMeshComponent->SetSimulatePhysics(true);
	m_pMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	RootComponent = m_pMeshComponent;
	
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	m_ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	m_ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("DecomposeUpdate"));
	FOnTimelineEvent DecomposeFinished;
	DecomposeFinished.BindUFunction(this, FName("DecomposeFinished"));

	m_DecompositionTimeLine.AddInterpFloat(m_pDecompositionCurve, ProgressUpdate);
	m_DecompositionTimeLine.SetTimelineFinishedFunc(DecomposeFinished);

	m_pMaterial = m_pMeshComponent->GetMaterial(0);
	m_pDynamicMaterial = UMaterialInstanceDynamic::Create(m_pMaterial, m_pMeshComponent);
	m_pMeshComponent->SetMaterial(0, m_pDynamicMaterial);
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(m_WeaponDurability <= 0)
	{
		UGameplayStatics::PlaySoundAtLocation(this, m_pWeaponBreakSound, GetActorLocation());
		m_ActorsToIgnore[0] = nullptr;
		m_pOwner->BreakWeapon();
		m_pOwner = nullptr;
		Destroy();
		m_bIsDecomposing = true;
	}
	if(m_bDecomposing)
	{		
		m_DecompositionTimeLine.TickTimeline(DeltaTime);		
	}
}

void ABaseWeapon::TraceAttack()
{	
	TArray<FHitResult> HitArray;

	if(const bool hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), m_pMeshComponent->GetSocketLocation(m_StartSocket), m_pMeshComponent->GetSocketLocation(m_EndSocket), m_WeaponRadius, m_ObjectTypes,
	                                                                     false, m_ActorsToIgnore, EDrawDebugTrace::None, HitArray, true))
	{
		if(Cast<AMainCharacter>(HitArray[0].GetActor())->IsValidLowLevel())
		{
			if(!m_pOwner->GetHasAttackHit())
			{
				Cast<AMainCharacter>(HitArray[0].GetActor())->DoDamageToSelf(m_WeaponDamage);
				m_pOwner->SetHasAttackHit(true);
				HitArray[0].Component->AddImpulseAtLocation(HitArray[0].Normal * m_Knockback, HitArray[0].Location, FName{"Head"});
				UGameplayStatics::PlaySoundAtLocation(this, m_pCutlassHitSound, GetActorLocation());
				UNiagaraComponent* niagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					m_pAttackHitNiagaraSystem,
					HitArray[0].Location,
					FRotator(1),
					FVector(1)
					);
				m_WeaponDurability--;
			}
		}		
	}
}

void ABaseWeapon::SetIsDecomposing(bool isDecomposing)
{
	m_bDecomposing = isDecomposing;
	m_DecompositionTimeLine.Play();
}


void ABaseWeapon::PickedUp(AMainCharacter* owner)
{
	m_ActorsToIgnore.Add(owner);
	m_pOwner = owner;
}

void ABaseWeapon::Dropped()
{
	m_ActorsToIgnore[0] = nullptr;
	m_pOwner = nullptr;
}

void ABaseWeapon::DecomposeUpdate(float DeltaTime)
{
	m_Decomposition = m_pDecompositionCurve->GetFloatValue(m_DecompositionTimeLine.GetPlaybackPosition());
	m_pDynamicMaterial->SetScalarParameterValue(TEXT("Burn Amount"), m_Decomposition);
}

void ABaseWeapon::DecomposeFinished()
{	
	Destroy();
}


