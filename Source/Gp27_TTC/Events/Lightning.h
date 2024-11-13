// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundBase.h"
#include "NiagaraComponent.h"
#include "Lightning.generated.h"




UCLASS()
class GP27_TTC_API ALightning : public AActor
{
	GENERATED_BODY()
	
public:
	ALightning();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	FVector m_Location;
	
	void SetNiagaraSystem(UNiagaraSystem* niagaraSystem);
	void SetSound(USoundBase* sound);
	void SetCameraShake(TSubclassOf<UCameraShakeBase> shake);
	void StrikeLightning(FVector location);
	void CheckStrike();

private:
	bool m_bIsCounting{false};
	
	const float m_LightningRadius{100.f};
	const float m_LightningDamage{150.f};
	float m_LightningLifeTime{};
	float m_LightningCounter{};
	
	USoundBase* m_pLightingStrikeSFX;
	UNiagaraSystem* m_pNiagaraSystemStrike;
	TSubclassOf<UCameraShakeBase> m_pCameraShake;
};
