// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Gp27_TTC/Events/LightningManager.h"
#include "Gp27_TTC/Events/MeshPointGetter.h"
#include "Gp27_TTC/Water/WaterLevel.h"
#include "CouchCoopGameMode.generated.h"

class APlayerSpawn;
class ADigSpotSpawner;
struct FTeamStats;
class AEndGameState;
class AMainGameState;
class AMainCharacter;
//Forward declarations
class APlayerStart;
class AWaterLevel;
class AMeshPointGetter;
class ALightningManager;

UCLASS()
class GP27_TTC_API ACouchCoopGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACouchCoopGameMode();

	void SetEndGameState(FTeamStats leftTeamStats, FTeamStats rightTeamStats) const;

	UFUNCTION(BlueprintCallable)
	void SetMainGameState() const;

protected:
	virtual void BeginPlay() override;

private:
	TArray<APlayerSpawn*> m_pPlayerStarts{};
	TArray<AMainCharacter*> m_pPlayers{};
	AWaterLevel* m_pWaterLevel;
	AMeshPointGetter* m_pLightningPointGetter;
	ALightningManager* m_pLightningManager;
	ADigSpotSpawner* m_pDigSpotSpawner;

	void SetMainCamera() const;
	void SpawnPlayers();

public: //Exposed variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Spawn Info", DisplayName = "Main Character Class")
	UClass* m_pMainCharacterClass{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Follow Camera", DisplayName = "Moving Camera Class")
	UClass* m_pMovingCameraClass{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Team Settings", DisplayName = "Blue Team Color")
	FLinearColor m_BlueTeamColor{0.0f, 0.0f, 1.0f, 1.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Team Settings", DisplayName = "Red Team Color")
	FLinearColor m_RedTeamColor{1.0f, 0.0f, 0.0f, 1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Team Settings", DisplayName = "Model With Hat")
	USkeletalMesh* m_pModelWithHat{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Team Settings", DisplayName = "Model Without Hat")
	USkeletalMesh* m_pModelWithoutHat{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Settings", DisplayName = "End Game State Class")
	UClass* m_pEndGameState{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Game Settings", DisplayName = "Main Game State Class")
	UClass* m_pMainGameState{};
};
