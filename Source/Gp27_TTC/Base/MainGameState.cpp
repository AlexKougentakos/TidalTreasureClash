// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"

#include "CouchCoopGameMode.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Gp27_TTC/Ship/Ship.h"
#include "Gp27_TTC/Treasure/Treasure.h"
#include "Gp27_TTC/Treasure/TreasureChest.h"
#include "Gp27_TTC/Water/WaterLevel.h"
#include "Kismet/GameplayStatics.h"

AMainGameState::AMainGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainGameState::BeginPlay()
{
	Super::BeginPlay();
	
	const TArray<AActor*> pShipActors = [&]() ->TArray<AActor*>
	{
		TArray<AActor*> pShipsTemp{};
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShip::StaticClass(), pShipsTemp);
		return  pShipsTemp;
	}();

	for (const auto& pShip : pShipActors)
	{
		if (AShip* pShips = Cast<AShip>(pShip))
		{
			pShips->OnShipLootScored.AddDynamic(this, &ThisClass::OnShipLootScored);
			pShips->OnShipLootStolen.AddDynamic(this, &ThisClass::OnShipLootStolen);
		}
	}

	TArray<AActor*> pWaterLevelTemp{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaterLevel::StaticClass(), pWaterLevelTemp);	
	m_pWaterLevel = Cast<AWaterLevel>(pWaterLevelTemp[0]);
	checkf(m_pWaterLevel, TEXT("No water level in this level"));
	m_pWaterLevel->OnFinalWaterLevelReached.AddDynamic(this, &ThisClass::OnFinalWaterLevelReached);
}

void AMainGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!m_bIsGameRunning) return;
	
	if(m_bHasDug)
	{
		m_CurrentWaterTime += DeltaSeconds;
        HandleWaterLevel();
	}

	//Print the team stats
	GEngine->AddOnScreenDebugMessage(11, 5.f, FColor::Red, FString::Printf(TEXT("Left Team Kills: %d"), m_LeftTeamStats.Kills));
	GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Red, FString::Printf(TEXT("Left Team Deaths: %d"), m_LeftTeamStats.Deaths));
	GEngine->AddOnScreenDebugMessage(13, 5.f, FColor::Red, FString::Printf(TEXT("Left Team Treasure Collected: %d"), m_LeftTeamStats.TreasureCollected));

	GEngine->AddOnScreenDebugMessage(14, 5.f, FColor::Red, FString::Printf(TEXT("Right Team Kills: %d"), m_RightTeamStats.Kills));
	GEngine->AddOnScreenDebugMessage(15, 5.f, FColor::Red, FString::Printf(TEXT("Right Team Deaths: %d"), m_RightTeamStats.Deaths));
	GEngine->AddOnScreenDebugMessage(16, 5.f, FColor::Red, FString::Printf(TEXT("Right Team Treasure Collected: %d"), m_RightTeamStats.TreasureCollected));
}

void AMainGameState::GetPlayers()
{
	const TArray<AActor*> pCharacterActors = [&]() ->TArray<AActor*>
	{
		TArray<AActor*> pCharactersTemp{};
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainCharacter::StaticClass(), pCharactersTemp);
		return  pCharactersTemp;
	}();

	for (const auto& pCharacter : pCharacterActors)
	{
		if (AMainCharacter* pCharacters = Cast<AMainCharacter>(pCharacter))
		{
			pCharacters->OnPlayerDeath.AddDynamic(this, &ThisClass::OnPlayerDeath);
			pCharacters->OnPlayerRespawn.AddDynamic(this, &ThisClass::OnPlayerRespawn);
			pCharacters->OnPlayerKill.AddDynamic(this, &ThisClass::OnPlayerKill);
		}
	}
}

void AMainGameState::HandleWaterLevel()
{
	if(m_CurrentWaterTime > m_StageTimer)
	{
		m_pWaterLevel->StartRisingWaterLevel();
        m_StageTimer += m_TimePerStage;				
	}
}

void AMainGameState::OnPlayerKill(AMainCharacter* pCharacter)
{
	if (pCharacter->GetTeamID() == 1)
		++m_LeftTeamStats.Kills;
	else
		++m_RightTeamStats.Kills;
}

void AMainGameState::OnShipLootStolen(int32 teamID,ATreasureChest* pTreasure)
{
	if (teamID == 1)
	{
		--m_LeftTeamScore;
		--m_LeftTeamStats.TreasureCollected;
	}
	else
	{
		--m_RightTeamScore;
		--m_RightTeamStats.TreasureCollected;
	}
	OnScoreChanged.Broadcast(m_LeftTeamScore, m_RightTeamScore);
	UGameplayStatics::PlaySoundAtLocation(this, m_pLootStolenSound, pTreasure->GetActorLocation());
	//Print out the scores of each team
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("Left Team Score: %d"), m_LeftTeamScore));
	GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, FString::Printf(TEXT("Right Team Score: %d"), m_RightTeamScore));
}

void AMainGameState::OnShipLootScored(int32 teamID, ATreasureChest* pTreasure)
{
	if (teamID == 1)
	{
		++m_LeftTeamScore;
		++m_LeftTeamStats.TreasureCollected;
	}
	else
	{
		++m_RightTeamScore;
		++m_RightTeamStats.TreasureCollected;
	}
	OnScoreChanged.Broadcast(m_LeftTeamScore, m_RightTeamScore);
	UGameplayStatics::PlaySoundAtLocation(this, m_pLootScoredSound, pTreasure->GetActorLocation());
	
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("Left Team Score: %d"), m_LeftTeamScore));
	GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, FString::Printf(TEXT("Right Team Score: %d"), m_RightTeamScore));
}

void AMainGameState::OnPlayerDeath(AMainCharacter* pCharacter)
{
	//Print name of the player who died
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("Player %s died"), *pCharacter->GetName()));
	
	pCharacter->Respawn(m_PlayerDeathTimer);
	
	if (pCharacter->GetTeamID() == 1)
		++m_LeftTeamStats.Deaths;
	else
		++m_RightTeamStats.Deaths;
	UGameplayStatics::PlaySoundAtLocation(this, m_pDeathSound, pCharacter->GetActorLocation());
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("Death")));
}

void AMainGameState::OnPlayerRespawn(AMainCharacter* pCharacter)
{
	UGameplayStatics::PlaySoundAtLocation(this, m_pRespawnSound, pCharacter->GetActorLocation());

}

void AMainGameState::OnFinalWaterLevelReached()
{
	GetWorld()->GetAuthGameMode<ACouchCoopGameMode>()->SetEndGameState(m_LeftTeamStats, m_RightTeamStats);
}

void AMainGameState::OnFirstDig()
{
	m_bHasDug = true;
}
