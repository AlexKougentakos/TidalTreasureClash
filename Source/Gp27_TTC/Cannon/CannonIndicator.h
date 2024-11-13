// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CannonIndicator.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;

UCLASS()
class GP27_TTC_API ACannonIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannonIndicator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetVisibility(bool visible);
	void UpdatePosition(FVector desiredLocation);
	const FVector GetTargetLocation();
	const FVector GetCapsuleLocation();
private:
	UCapsuleComponent* m_pTargetCapsuleComponent{};
	UNiagaraComponent* m_pTargetNiagaraSystem{};
	UNiagaraComponent* m_pTargetNiagaraSystem2{};
};
