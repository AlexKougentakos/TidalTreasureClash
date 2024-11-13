// Fill out your copyright notice in the Description page of Project Settings.


#include "CannonIndicator.h"

#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ACannonIndicator::ACannonIndicator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACannonIndicator::BeginPlay()
{
	Super::BeginPlay();
	m_pTargetNiagaraSystem = FindComponentByTag<UNiagaraComponent>(FName{"Indicator"});
	m_pTargetNiagaraSystem2 = FindComponentByTag<UNiagaraComponent>(FName{"Indicator2"});
	m_pTargetCapsuleComponent = FindComponentByClass<UCapsuleComponent>();
}

// Called every frame
void ACannonIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACannonIndicator::SetVisibility(bool visible)
{	
	m_pTargetNiagaraSystem->SetVisibility(visible);	
	m_pTargetNiagaraSystem2->SetVisibility(visible);
}

void ACannonIndicator::UpdatePosition(FVector desiredLocation)
{
	m_pTargetCapsuleComponent->SetWorldLocation(desiredLocation);
	FHitResult hitResult{};
	GetWorld()->LineTraceSingleByChannel(hitResult, desiredLocation, desiredLocation - FVector{0,0,2500}, ECC_WorldStatic);
	m_pTargetNiagaraSystem->SetWorldLocation(FVector{desiredLocation.X, desiredLocation.Y, hitResult.Location.Z + 25.f});
		
}

const FVector ACannonIndicator::GetTargetLocation()
{
	return m_pTargetNiagaraSystem->GetComponentLocation();
}

const FVector ACannonIndicator::GetCapsuleLocation()
{
	return m_pTargetCapsuleComponent->GetComponentLocation();
}

