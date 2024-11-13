// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Gp27_TTC/Events/Lightning.h"
#include "Gp27_TTC/Events/MeshPointGetter.h"
#include "MainGameState.generated.h"

class AWaterLevel;

UCLASS()
class GP27_TTC_API AMainGameState : public AGameState
{
	GENERATED_BODY()

public:
	AMainGameState();
	
	UFUNCTION()
	void OnShipLootStolen(int32 teamID);
	UFUNCTION()
	void OnShipLootScored(int32 teamID);
	UFUNCTION()
	void OnPlayerDeath(AMainCharacter* pCharacter);
	UFUNCTION()
	void OnPlayerRespawn(AMainCharacter* pCharacter);
	UFUNCTION()
	void OnFinalWaterLevelReached();
	UFUNCTION()
	void OnFirstDig();

	UFUNCTION()
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void Tick(float DeltaSeconds) override;

private:	
	
	int m_LeftTeamScore{};
	int m_RightTeamScore{};

	float m_CurrentWaterTime{};

	const float m_PlayerDeathTimer{5.f};

	float m_StageTimer{10.f};
	const float m_TimePerStage{10.f};

	AWaterLevel* m_pWaterLevel{};
	
	bool m_bHasDug{true};

	void HandleWaterLevel();
};
