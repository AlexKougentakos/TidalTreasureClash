// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Character.h"
#include "Gp27_TTC/Weapons/BaseWeapon.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Gp27_TTC/CustomAnimInstance.h"
#include "MainCharacter.generated.h"

class AInteractable;
class ACannon;
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeath, AMainCharacter*, MainCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeath, AMainCharacter*, PlayerWhoDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerRespawn, AMainCharacter*, MainCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerKill, AMainCharacter*, PlayerWhoGotTheKill);

UCLASS()
class GP27_TTC_API AMainCharacter : public ACharacter, public IWaterInteractable
{
	GENERATED_BODY()

public:
	AMainCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float deltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetRespawnLocation(const FTransform& transform) { m_pRespawnLocationTransform = &transform; }
	
	void Respawn(const float respawnTime);

	void InteractPressed();
	UFUNCTION()
	void InteractReleased();
	void InteractHeld();

	void FireCannon();
	void ReleaseCannon();
	void MoveCannon(FVector2D targetMovement);

	virtual void SetIsDecomposing(bool isDecomposing) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "decomposition", DisplayName = "Decomposition curve")
	UCurveFloat* m_pDecompositionCurve;
	
	/// @return Did we pick up the actor?
	bool HandlePickUp();
	void Drop();

	bool GetIsHolding() const;
	bool GetIsHoldingWeapon() const;
	bool GetIsAttacking() const;
	bool GetHasAttackHit() const;
	bool IsDigging() const { return  m_bIsDigging;}
	int GetTeamID() const {return m_PlayerTeamID; }
	void SetTeamID(const int teamID) {m_PlayerTeamID = teamID; }
	
	void SetHasAttackHit(bool hasHit);
	void SetIsAttacking();
	void TraceAttack();
	void BreakWeapon();
	
	//Return: Did player die?
	bool DoDamageToSelf(float damage);
	ABaseWeapon* GetHeldWeapon() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Character Settings", DisplayName = "Drop Distance")
	float m_DropDistance{50.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character Settings", DisplayName = "Attack particles")
	UNiagaraSystem* m_pAttackHitNiagaraSystem;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FPhysicalAnimationData m_PhysicalAnimationData;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    UPhysicalAnimationComponent* m_pPhysicalAnimationComponent{};
	
	FOnPlayerDeath OnPlayerDeath;
	FOnPlayerRespawn OnPlayerRespawn;
	FOnPlayerKill OnPlayerKill;
	int m_AmounbtOfDrops;
	

private:
	bool m_bIsHolding{false};
	bool m_bIsHoldingWeapon{false};
	bool m_bIsAttacking{false};
	bool m_bHasAttackHit{false};
	bool m_bIsDigging{false};
	bool m_bIsRagdoll{false};

	int m_PlayerTeamID{};
	
	UCustomAnimInstance* m_pAnimationInstance;
	
	ABaseWeapon* m_pHeldWeapon;
	
	//add references for every type of object that can be picked up
	
	UStaticMeshComponent* m_pHeldMeshComponent{};

	USkeletalMeshComponent* m_pSkeletalMesh{};
		
	TArray<TEnumAsByte<EObjectTypeQuery>> m_ObjectTypes{};
	TArray<TEnumAsByte<EObjectTypeQuery>> m_AttackObjectTypes{};
	FName m_WeaponSocketName{"RightHandSocket"};
	FName m_ItemSocketName{"Spine1Socket"};
	FName m_Body{"Hips"};
	
	TArray<AActor*> m_AttackActorsToIgnore;

	ACannon* m_pInteractedCannon{};

	//============================================================
	//						PLAYER STATS
	//============================================================
	float m_Health{100.f};
	const float m_MaxHealth{100.f};

	const float m_Knockback{-20000.f};
	
	const float m_FistDamage{25.f};
	const float m_FistRadius{20.f};
	const float m_PickupRadius{90.f};

	//============================================================
	//						RESPAWNING
	//============================================================
	float m_ElapsedRespawnTime{};
	float m_TotalRespawnTime{};
	bool m_IsRespawning{false};
	const FTransform* m_pRespawnLocationTransform{};

	//============================================================
	//						INTERACTING
	//============================================================
	 bool m_bDidInitialCheckForInteraction{false}; // We should only do the sphere trace once when the player holds the interact button
	AInteractable* m_pCurrentHeldInteractable{};
	
	//============================================================
	//						SOUND
	//============================================================
	UPROPERTY(EditDefaultsOnly, Category = "Sound", DisplayName = "Hit Sound")
	USoundBase* m_pHitSound{};

	//decomposition animation
	bool m_bDecomposing{false};
	float m_Decomposition{};
	
	UFUNCTION()
	void DecomposeUpdate(float DeltaTime);
	UFUNCTION()
	void DecomposeFinished();
	
	UMaterialInterface* m_pMaterial;
	UMaterialInstanceDynamic* m_pDynamicMaterial;
	FTimeline m_DecompositionTimeLine;
};
