// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "NiagaraComponent.h"
#include "LightningManager.generated.h"

class AMeshPointGetter;

UCLASS()
class GP27_TTC_API ALightningManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALightningManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void SetPointGetter(AMeshPointGetter* pointGetter);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", DisplayName = "Lighting Strike Niagara System")
	UNiagaraSystem* m_pNiagaraSystemStrike;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", DisplayName = "Lighting Strike SFX")
	USoundBase* m_pLightningStrikeSFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", DisplayName = "Lightning Shake Blueprint")
	TSubclassOf<UCameraShakeBase> m_pLightningShake;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning event settings", DisplayName = "Lighning Interval")
	float m_LightningInterval{5.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning event settings", DisplayName = "Lightning Intensity")
	int m_LightningIntensity{20};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning event settings", DisplayName = "Event Interval Min")
	float m_LightningEventIntervalMin{10.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning event settings", DisplayName = "Event Interval Max")
	float m_LightningEventIntervalMax{20.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning event settings", DisplayName = "Event Duration Min")
	float m_EventDurationMin{10.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lightning event settings", DisplayName = "Event Duration Max" )
	float m_EventDurationMax{20.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor references", DisplayName = "Point Getter")
	AMeshPointGetter* m_pLightningPointGetter;

	void SetLightningEnabled(const bool enabled) {m_bCanStrike = enabled;}

private:
	bool m_bIsEventStarted{false};
	bool m_bCanStrike{false};

	float m_EventDuration{};
	float m_EventTimer{};
	float m_EventTimerActive{};
	float m_EventInterval{};
	float m_LightningTimer{};

	
	TArray<FVector> m_LightningPoints;
};


