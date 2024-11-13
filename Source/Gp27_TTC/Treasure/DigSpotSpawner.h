#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DigSpotSpawner.generated.h"

UCLASS()
class GP27_TTC_API ADigSpotSpawner final : public AActor
{
	GENERATED_BODY()
	
public:	
	ADigSpotSpawner();

	virtual void Tick(float deltaTime) override;
	virtual void BeginPlay() override;

	// ==================================================
	//					Settings
	// ==================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DigSpot Spawner Settings", DisplayName = "Minimum Delay Between Treasures")
	float m_MinimumDelayBetweenSpawns{10.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DigSpot Spawner Settings", DisplayName = "Maximum Delay Between Treasures")
	float m_MaximumDelayBetweenSpawns{20.0f};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DigSpot Spawner Settings", DisplayName = "Don't Spawn Until Opened")
	bool m_bDontSpawnUntilOpened{true};
	
	// ==================================================
	//					Parameters
	// ================================================== 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DigSpot Spawner Parameters", DisplayName = "DigSpot Location Class")
	UClass* m_pDigSpotLocationClass{};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DigSpot Spawner Parameters", DisplayName = "DigSpot Class")
	UClass* m_pDigSpotClass{};
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="DigSpot Spawner Parameters", DisplayName = "Water Plane")
	AActor* m_pWaterPlane{};

	UFUNCTION()
	void OnTreasureOpened();
	void SpawnTreasure();
	
private:	
	TArray<FVector> m_DigSpots{};
	float m_TimeSinceLastSpawn{};
	float m_CurrentDelayBetweenSpawns{};
	bool m_bWasPreviousTreasureOpened{true}; //Start with true so that we can spawn the first treasure
	
	
	void GetDigSpotLocations();
	void RemovePointsUnderTheWater();
	FVector SelectSpawningLocation();
	bool CanSpawnTreasure() const;
};
