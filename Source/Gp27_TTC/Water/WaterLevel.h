// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "WaterLevel.generated.h"

class UNiagaraSystem;
class AShip;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinalWaterLevelReached);

UCLASS()
class GP27_TTC_API AWaterLevel : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWaterLevel();

	FOnFinalWaterLevelReached OnFinalWaterLevelReached;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ship Settings", DisplayName = "Ship team 1")
	AShip* m_pShip1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ship Settings", DisplayName = "Ship team 2")
	AShip* m_pShip2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ship Settings", DisplayName = "Min Ship Z Position")
	float m_MinShipZPosition{-30.f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ship Settings", DisplayName = "Max Ship Z Position")
	float m_MaxShipZPosition{85.f};
		
	UPROPERTY(VisibleAnywhere, DisplayName = "Water Plane Mesh")
	UStaticMeshComponent* m_pMeshComponent{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", DisplayName = "Camera Shake")
	TSubclassOf<UCameraShakeBase> m_pCameraShake{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX", DisplayName = "Water Splash Niagara System")
	UNiagaraSystem* m_pWaterSplashNiagaraSystem{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Water Level Settings", DisplayName = "Water Level Stages")
	TArray<float> m_WaterLevelStages;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Water Level Settings", DisplayName = "Max Water Speed")
	float m_WaterMaxSpeed{20};


	UFUNCTION()
	void StartRisingWaterLevel();

	void GetPlayers();
	UFUNCTION(BlueprintCallable)
	float GetCurrentWaterLevel();
	bool PassedHalfStage() const;

private:
	bool m_bIsWaterLevelRising{false};

	int m_CurrentStage{};
	int m_MaximumStage{};
	float m_StartingWaterLevel{};

	float m_ZDifferenceShip1{};
	float m_ZDifferenceShip2{};

	const float m_DrowningDifference{5.f};

	TArray<AActor*> m_pCharacterActors;

	void UpdatePosition(float DeltaTime);
	void CheckDrowning();

	UFUNCTION()
	void OnCollisionWithActors(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                           const FHitResult& SweepResult);
	
	 UPROPERTY(EditAnywhere, Category = "Sound", DisplayName = "Water Rising Sound")
	 USoundBase* m_pWaterRisingSound{};
	 float m_WaterRisingSoundVolume{0.001f};
};
