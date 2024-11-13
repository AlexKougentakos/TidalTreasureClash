#include "MainCharacter.h"

#include "MainPlayerController.h"
#include "NiagaraFunctionLibrary.h"
#include "Gp27_TTC/Base/Interactable.h"
#include "Gp27_TTC/Cannon/Cannon.h"
#include "Gp27_TTC/Treasure/DigSpot.h"
#include "Gp27_TTC/Weapons/WeaponRack.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

#define ECC_InteractionChannel ECC_GameTraceChannel1
#define ECC_PickupChannel ECC_GameTraceChannel2

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	m_pPhysicalAnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("Physical Animation Component"));

	m_bDeleteWhenInWater = false;
	m_SplashFactor = 1.75f;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	//add all types of objects we want the player to be able to pick up
	m_ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	m_ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_InteractionChannel));

	m_AttackObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	m_AttackObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	
	m_pSkeletalMesh = GetComponentByClass<USkeletalMeshComponent>();
	//Setting up the Physics Animation Component
	m_pPhysicalAnimationComponent->SetSkeletalMeshComponent(m_pSkeletalMesh);
	//Setting each body below the hips to simulate physics
	//We start with index 1 to skip the first body which is the hips, if the hips get simulated the mesh would fully ragdoll
	for(int index{1}; index < m_pSkeletalMesh->Bodies.Num(); index++)
	{
		m_pSkeletalMesh->Bodies[index]->SetInstanceSimulatePhysics(true);		
	}
	//Applying the settings for the physical animation
	m_pPhysicalAnimationComponent->ApplyPhysicalAnimationSettingsBelow(m_Body, m_PhysicalAnimationData, true);

	m_pAnimationInstance = Cast<UCustomAnimInstance>(m_pSkeletalMesh->GetAnimInstance());
	m_AttackActorsToIgnore.Add(this);

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("DecomposeUpdate"));
	FOnTimelineEvent DecomposeFinished;
	DecomposeFinished.BindUFunction(this, FName("DecomposeFinished"));

	m_DecompositionTimeLine.AddInterpFloat(m_pDecompositionCurve, ProgressUpdate);
	m_DecompositionTimeLine.SetTimelineFinishedFunc(DecomposeFinished);

	m_pMaterial = m_pSkeletalMesh->GetMaterial(0);
	m_pDynamicMaterial = UMaterialInstanceDynamic::Create(m_pMaterial, m_pSkeletalMesh);
	m_pSkeletalMesh->SetMaterial(0, m_pDynamicMaterial);

}

void AMainCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	if(m_Health <= 0)
	{
		GEngine->AddOnScreenDebugMessage(20, 5.f, FColor::Red, "Player Died! MainChearacter.cpp - Ln51");
		OnPlayerDeath.Broadcast(this);
		Drop();		
		Cast<AMainPlayerController>(GetController())->SetInputToCharacterMovement();
		DisableInput(Cast<AMainPlayerController>(GetController()));
		m_bDecomposing = true;
		m_DecompositionTimeLine.PlayFromStart();
		
	}
	if (m_IsRespawning)
	{
		m_ElapsedRespawnTime += deltaTime;
		if (m_ElapsedRespawnTime >= m_TotalRespawnTime)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Respawned!");
			m_bDecomposing = false;
			m_pDynamicMaterial->SetScalarParameterValue(TEXT("Burn Amount"), 0);
			m_Decomposition = 0;
			m_DecompositionTimeLine.Stop();
			m_IsRespawning = false;
			m_ElapsedRespawnTime = 0.f;
			SetActorLocation(m_pRespawnLocationTransform->GetLocation());
			EnableInput(Cast<AMainPlayerController>(GetController()));
			OnPlayerRespawn.Broadcast(this);
		}
	}
	if(m_bDecomposing)
	{
		m_DecompositionTimeLine.TickTimeline(deltaTime);	
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AMainCharacter::Respawn(const float respawnTime)
{	
	m_bIsRagdoll = false;
	m_IsRespawning = true;
	m_TotalRespawnTime = respawnTime;
	m_Health = 100.f;
}

void AMainCharacter::InteractPressed()
{
    const FVector start = GetActorLocation();
    const FVector end = GetActorLocation();

    FHitResult hitResult;
    GetWorld()->SweepSingleByChannel(
        hitResult,
        start,
        end,
        FQuat::Identity,
        ECC_GameTraceChannel1, // Your custom channel
        FCollisionShape::MakeSphere(m_PickupRadius),
        FCollisionQueryParams{}
    );

    if (!hitResult.bBlockingHit) return;

		const auto interactableActor = hitResult.GetActor();
    if (!interactableActor->GetClass()->IsChildOf(AInteractable::StaticClass())) return;
	if(interactableActor->GetClass()->IsChildOf(ACannon::StaticClass()))
	{
		Cast<ACannon>(interactableActor)->SetCharacter(this);
		m_pInteractedCannon = Cast<ACannon>(interactableActor);
	}
    const auto interactable = Cast<AInteractable>(interactableActor);

	if (interactable->GetInteractableType() != InteractableType::PRESSED) return;
    	interactable->Interact();

	
}

void AMainCharacter::InteractReleased()
{
	if (m_bDidInitialCheckForInteraction)
		m_bDidInitialCheckForInteraction = false;

	if (m_pCurrentHeldInteractable)
	{
		m_pCurrentHeldInteractable->InteractStop();
		m_pCurrentHeldInteractable = nullptr;
	}

	//These will always be turned to false when the player is done interacting no matter what so no need for checks
	m_pAnimationInstance->IsDigging = false;
	m_bIsDigging = false;

	bUseControllerRotationYaw = true;
}

void AMainCharacter::InteractHeld()
{
	if (m_bDidInitialCheckForInteraction) return;
	m_bDidInitialCheckForInteraction = true;

	const FVector start = GetActorLocation();
	const FVector end = GetActorLocation();

	FHitResult hitResult;
	GetWorld()->SweepSingleByChannel(
		hitResult,
		start,
		end,
		FQuat::Identity,
		ECC_GameTraceChannel1, // Your custom channel
		FCollisionShape::MakeSphere(m_PickupRadius),
		FCollisionQueryParams{}
	);

	if (!hitResult.bBlockingHit) return;

	const auto interactableActor = hitResult.GetActor();
	if (!interactableActor->GetClass()->IsChildOf(AInteractable::StaticClass())) return;

	const auto interactable = Cast<AInteractable>(interactableActor);
	if (interactable->GetInteractableType() != InteractableType::HELD) return;

	m_pCurrentHeldInteractable = interactable;
	interactable->Interact();

	if (interactable->IsA(ADigSpot::StaticClass()) && interactable->IsInteractable())
	{
		ADigSpot* digSpot = Cast<ADigSpot>(interactable);
		if (!digSpot->OnInteractionCompleted.IsAlreadyBound(this, &AMainCharacter::InteractReleased))
		{
			digSpot->OnInteractionCompleted.AddDynamic(this, &AMainCharacter::InteractReleased);
		}
		m_pAnimationInstance->IsDigging = true;
		bUseControllerRotationYaw = false;
		m_bIsDigging = true;
		
		// Rotate the player around the Z axis to look towards the interactable
		FVector PlayerLocation = GetActorLocation();
		FVector TargetLocation = interactable->GetActorLocation();
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TargetLocation);

		// Correct for the 90 degree offset in the Yaw component
		float CorrectedYaw = LookAtRotation.Yaw - 90.0f;

		// Set only the Yaw component, maintaining the current Pitch and Roll
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FRotator(CurrentRotation.Pitch, CorrectedYaw, CurrentRotation.Roll);
		SetActorRotation(NewRotation);

	}
}

void AMainCharacter::FireCannon()
{
	m_pInteractedCannon->Fire();
}

void AMainCharacter::ReleaseCannon()
{
	if(m_pInteractedCannon)
	{
		m_pInteractedCannon->ResetCharachterRef();
	}
	m_pInteractedCannon = nullptr;
}

void AMainCharacter::MoveCannon(FVector2D targetMovement)
{
	if(m_pInteractedCannon)
	{
		m_pInteractedCannon->MoveTarget(targetMovement);
	}
}

void AMainCharacter::SetIsDecomposing(bool isDecomposing)
{
	m_bDecomposing = isDecomposing;
	m_DecompositionTimeLine.Play();
}


bool AMainCharacter::HandlePickUp()
{
	m_AmounbtOfDrops++;
	if (m_bIsHolding) //Todo: Rework this logic
	{
		Drop();
		return false;
	}
	
	const FHitResult hitResult = [&]() -> FHitResult
	{
		const FVector start = GetActorLocation();
		const FVector end = GetActorLocation();
		
		FHitResult _hitResult;
		GetWorld()->SweepSingleByChannel(
		_hitResult, 
		start, 
		end, 
		FQuat::Identity, 
		ECC_PickupChannel, // Your custom channel
		FCollisionShape::MakeSphere(m_PickupRadius),
		FCollisionQueryParams{}
		);
		
		return _hitResult;
	}();

	if (!hitResult.bBlockingHit) return false;
	
	const auto pickUp = hitResult.GetActor();
	const auto component = hitResult.Component;
	
	if (m_bIsHolding) return false;
	
	m_bIsHolding = true;
	
	//Is weapon
	if(pickUp->GetClass()->IsChildOf(ABaseWeapon::StaticClass()))
	{
		m_pHeldWeapon = Cast<ABaseWeapon>(pickUp);
		m_pHeldWeapon->PickedUp(this);
		m_bIsHoldingWeapon = true;				
	}
	else
	{
		m_bIsHoldingWeapon = false;
		m_pAnimationInstance->IsCarrying = true;
	}		

	m_pHeldMeshComponent = Cast<UStaticMeshComponent>(component);

	m_pHeldMeshComponent->SetSimulatePhysics(false);
	m_pHeldMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	if(m_bIsHoldingWeapon)
	{
		m_pHeldWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, m_WeaponSocketName);
	}
	else
	{
		m_pHeldMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, m_ItemSocketName);
	}

	return true;
}

void AMainCharacter::Drop()
{
	
	if(!m_bIsHolding) return;

	m_bIsHolding = false;
	m_pAnimationInstance->IsCarrying = false;

	//Get forward vector of the player
	const FVector forwardDirection = GetActorRightVector();
	FVector dropLocation = GetActorLocation() + forwardDirection * m_DropDistance;
	dropLocation.Z += 50.f;
	if(m_bIsHoldingWeapon)
	{
		m_pHeldWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		m_pHeldWeapon->Dropped();
		m_pHeldWeapon->SetActorLocation(dropLocation);
		m_pHeldWeapon = nullptr;
		m_bIsHoldingWeapon = false;
	}
	else
	{
		m_pHeldMeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		m_pHeldMeshComponent->GetOwner()->SetActorLocation(dropLocation);
	}

	m_pHeldMeshComponent->SetSimulatePhysics(true);
	m_pHeldMeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
	m_pHeldMeshComponent->SetCollisionResponseToChannel(ECC_PickupChannel, ECR_Block);
	m_pHeldMeshComponent = nullptr;
}

bool AMainCharacter::GetIsHolding() const
{
	return m_bIsHolding;
}

bool AMainCharacter::GetIsHoldingWeapon() const
{
	return m_bIsHoldingWeapon;
}

bool AMainCharacter::GetIsAttacking() const
{
	return m_bIsAttacking;
}

bool AMainCharacter::GetHasAttackHit() const
{
	return m_bHasAttackHit;
}

void AMainCharacter::SetHasAttackHit(bool hasHit)
{
	m_bHasAttackHit = hasHit;
}

void AMainCharacter::SetIsAttacking()
{	
	m_bIsAttacking = !m_bIsAttacking;
	m_pAnimationInstance->IsAttacking = m_bIsAttacking;
}

void AMainCharacter::TraceAttack()
{	
	if(m_pHeldWeapon != nullptr)
	{
		m_pHeldWeapon->TraceAttack();
	}
	else
	{
		TArray<FHitResult> HitArray;

		if(const bool hit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), m_pSkeletalMesh->GetSocketLocation(m_WeaponSocketName), m_pSkeletalMesh->GetSocketLocation(m_WeaponSocketName), m_FistRadius, m_AttackObjectTypes,
		                                                                     false, m_AttackActorsToIgnore, EDrawDebugTrace::None, HitArray, true))
		{
			if(Cast<AMainCharacter>(HitArray[0].GetActor())->IsValidLowLevel())
			{
				if(!m_bHasAttackHit)
				{
					if (Cast<AMainCharacter>(HitArray[0].GetActor())->DoDamageToSelf(m_FistDamage))
						OnPlayerKill.Broadcast(this);
					
					m_bHasAttackHit = true;
					UGameplayStatics::PlaySoundAtLocation(this, m_pHitSound, GetActorLocation());
					HitArray[0].Component->AddImpulseAtLocation(HitArray[0].Normal * m_Knockback, HitArray[0].Location, FName{"Head"});
					UNiagaraComponent* niagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						GetWorld(),
						m_pAttackHitNiagaraSystem,
					HitArray[0].Location,
					FRotator(1),
					FVector(1)
		 );
				}
			}		
		}
	}
}

void AMainCharacter::BreakWeapon()
{
	m_pHeldWeapon = nullptr;
	m_bIsHolding = false;	
}

bool AMainCharacter::DoDamageToSelf(float damage)
{
	m_Health -= damage;
	return m_Health <= 0;
}

ABaseWeapon* AMainCharacter::GetHeldWeapon() const
{
	return m_pHeldWeapon;
}

void AMainCharacter::DecomposeUpdate(float DeltaTime)
{
	m_Decomposition = m_pDecompositionCurve->GetFloatValue(m_DecompositionTimeLine.GetPlaybackPosition());
	m_pDynamicMaterial->SetScalarParameterValue(TEXT("Burn Amount"), m_Decomposition);
}

void AMainCharacter::DecomposeFinished()
{
	SetActorLocation({10000.f, 50.f, 1000.f});
}


