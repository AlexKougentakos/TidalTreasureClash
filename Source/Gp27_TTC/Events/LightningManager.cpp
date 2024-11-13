// Fill out your copyright notice in the Description page of Project Settings.


#include "LightningManager.h"

#include "Lightning.h"
#include "MeshPointGetter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALightningManager::ALightningManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
}

// Called when the game starts or when spawned
void ALightningManager::BeginPlay()
{
	Super::BeginPlay();

	m_EventInterval = FMath::FRandRange(m_LightningEventIntervalMin , m_LightningEventIntervalMax);
}

// Called every frame
void ALightningManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_bCanStrike) return;
	
	m_EventTimer += DeltaTime;
	if(m_EventTimer >= m_EventInterval)
	{
		//todo: change post processing to stormy
		m_bIsEventStarted = true;
		m_EventDuration = FMath::FRandRange(m_EventDurationMin, m_EventDurationMax);
		GEngine->AddOnScreenDebugMessage(12, -1, FColor::Orange, "LightningEvent started");
	}
	if(m_bIsEventStarted)
	{
		GEngine->AddOnScreenDebugMessage(13, -1, FColor::Orange, "LightningEvent tick");

		m_EventTimerActive += DeltaTime;
		m_LightningTimer += DeltaTime;
		if(m_LightningTimer >= m_LightningInterval)
		{
			GEngine->AddOnScreenDebugMessage(11, -1, FColor::Orange, " before getting points");
			if(m_pLightningPointGetter)
			{
				GEngine->AddOnScreenDebugMessage(11, -1, FColor::Orange, " point getter pointer ok");
			}
			m_LightningPoints = m_pLightningPointGetter->SelectPoints(m_LightningIntensity);
			GEngine->AddOnScreenDebugMessage(10, -1, FColor::Orange, "after getting points");
			for (int index{}; index < m_LightningPoints.Num(); index++)
			{
				GEngine->AddOnScreenDebugMessage(16, -1, FColor::Orange, "Lightningstrikes started");
			
				const FVector location = m_LightningPoints[index];
				ALightning* lightning = GetWorld()->SpawnActor<ALightning>();
				GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Red, FString::Printf(TEXT("lightning strike %f %f %f"), location.X, location.Y, location.Z ));
				lightning->SetNiagaraSystem(m_pNiagaraSystemStrike);
				lightning->SetSound(m_pLightningStrikeSFX);
				lightning->SetCameraShake(m_pLightningShake);
				lightning->StrikeLightning(location);
			}
			m_LightningTimer = 0;
		}
		if(m_EventTimerActive >= m_EventDuration)
		{
			//todo: change post processing to normal
			m_bIsEventStarted = false;
			m_EventInterval = FMath::FRandRange(m_LightningEventIntervalMin , m_LightningEventIntervalMax);
			m_EventTimer = 0.f;
			m_EventTimerActive = 0.f;
		}
	}
	
}

void ALightningManager::SetPointGetter(AMeshPointGetter* pointGetter)
{	
	m_pLightningPointGetter = pointGetter;	
}

