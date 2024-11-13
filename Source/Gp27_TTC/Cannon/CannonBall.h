// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Gp27_TTC/Water/IWaterInteractable.h"
#include "CannonBall.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
UCLASS()
class GP27_TTC_API ACannonBall : public AActor, public IWaterInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannonBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", DisplayName = "Cannon ball explosion niagara system")
	UNiagaraSystem* m_pExplosionNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", DisplayName = "Camera Shake")
	TSubclassOf<UCameraShakeBase> m_pCameraShake;
	
	UPROPERTY(EditAnywhere, Category = "Sound", DisplayName = "Cannonball explosion sound")
	USoundBase* m_pCannonBallExplosionSound{};
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UProjectileMovementComponent* m_pProjectileMovement;

	float m_ExplosionRadius{150.f};
	float m_ExplosionDamage{100.f};


};
