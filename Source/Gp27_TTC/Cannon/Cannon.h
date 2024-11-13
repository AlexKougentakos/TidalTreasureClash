// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CannonBall.h"
#include "NiagaraSystem.h"
#include "Gp27_TTC/Base/Interactable.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Cannon.generated.h"

class ACannonIndicator;
class AMainCharacter;
class USphereComponent;
class UCapsuleComponent;
class UWidgetComponent;
class UCameraComponent;
class UNiagaraComponent;
class AMeshPointGetter;

UCLASS()
class GP27_TTC_API ACannon : public AInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Interact() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="indicator Settings", DisplayName = "Indicator cannon")
	ACannonIndicator* m_pIndicator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon mesh references", DisplayName = "base")
	UStaticMeshComponent* m_pBaseCannon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon mesh references", DisplayName = "barrel")
	UStaticMeshComponent* m_pBarrelCannon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon settings", DisplayName = "CannonBall BP class references")
	TSubclassOf<AActor> m_CannonBallClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon settings", DisplayName = "Cannon Shot niagara system")
	UNiagaraSystem* m_pCannonShotNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon settings", DisplayName = "Shooting Delay")
	float m_ShootingDelay{5.f};
	
	UPROPERTY(EditAnywhere, Category = "Cannon settings", DisplayName = "Cannonshot Sound")
	USoundBase* m_pCannonShotSound{};

	UPROPERTY(EditAnywhere, Category = "Cannon settings", DisplayName = "Camera Shake")
	TSubclassOf<UCameraShakeBase> m_pCameraShake{};

	UPROPERTY(EditAnywhere, Category = "Cannon settings", DisplayName = "CannonReload Sound")
	USoundBase* m_pCannonReloadSound{};
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RotateInteractionPrompt();
	void SetCharacter(AMainCharacter* characterRef);
	void ResetCharachterRef();
	void MoveTarget(FVector2D targetMovement);
	void Fire();
private:
	bool m_bUpdateTarget{false};
	bool m_bReadyToFire{false};
	

	float m_TargetSpeed{1000};
	float m_TargetLimitX{};
	float m_TargetLimitY{};	
	float m_ShootingTimer{};

	FVector2D m_DesiredDirection{};
	
	AMainCharacter* m_pInteractingCharacter;
	UWidgetComponent* m_pWidgetComponent{};
	UCameraComponent* m_pCameraComponent{};
	USphereComponent* m_pPickupRadiusSphereComponent{};	
	UNiagaraComponent* m_pFuseNiagaraSystem{};
	AMeshPointGetter* m_pPointGetter;
};


