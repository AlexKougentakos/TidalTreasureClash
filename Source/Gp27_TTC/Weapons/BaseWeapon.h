// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Gp27_TTC/Water/IWaterInteractable.h"
#include "BaseWeapon.generated.h"


class AMainCharacter;
class UNiagaraSystem;
class UMaterialInterface;
class UMaterialInstanceDynamic;

UCLASS()
class GP27_TTC_API ABaseWeapon : public AActor, public IWaterInteractable
{
	GENERATED_BODY()
	
public:	
	ABaseWeapon();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* m_pMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Main Character Settings", DisplayName = "Attack particles")
	UNiagaraSystem* m_pAttackHitNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "decomposition", DisplayName = "Decomposition curve")
	UCurveFloat* m_pDecompositionCurve;
	
	virtual void TraceAttack();
	virtual void SetIsDecomposing(bool isDecomposing) override;
	virtual void PickedUp(AMainCharacter* owner);
	virtual void Dropped();
	
private:
	FName m_StartSocket{"Start"};
	FName m_EndSocket{"End"};

	bool m_bDecomposing{false};
	float m_Decomposition{};
	float m_WeaponRadius{20.f};
	float m_WeaponDamage{40.f};
	int m_WeaponDurability{6};
	float m_Knockback{-40000.f};
	UPROPERTY(EditAnywhere, Category = "Sound", DisplayName = "Weapon Break Sound")
	USoundBase* m_pWeaponBreakSound{};
	UPROPERTY(EditAnywhere, Category = "Sound", DisplayName = "Cutlass Hit Sound")
	USoundBase* m_pCutlassHitSound{};
	TArray<TEnumAsByte<EObjectTypeQuery>> m_ObjectTypes{};
	
	TArray<AActor*> m_ActorsToIgnore;
	AMainCharacter* m_pOwner;

	UFUNCTION()
	void DecomposeUpdate(float DeltaTime);
	UFUNCTION()
	void DecomposeFinished();
	
	UMaterialInterface* m_pMaterial;
	UMaterialInstanceDynamic* m_pDynamicMaterial;
	FTimeline m_DecompositionTimeLine;
                       	
};
