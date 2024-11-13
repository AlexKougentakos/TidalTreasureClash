#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gp27_TTC/Base/Interactable.h"
#include "Gp27_TTC/Water/IWaterInteractable.h"
#include "Components/TimelineComponent.h"
#include "TreasureChest.generated.h"

class UCurveFloat;
class UMaterialInterface;
class UMaterialInstanceDynamic;
USTRUCT(BlueprintType)
struct FSpawnItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Item")
	int32 spawnChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Item")
	TSubclassOf<UObject> objectToSpawn;
	
};

UCLASS()
class GP27_TTC_API ATreasureChest : public AActor, public IWaterInteractable
{
	GENERATED_BODY()
	
public:
	ATreasureChest();
	

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void SetIsDecomposing(bool isDecomposing) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Treasure Chest Settings", DisplayName = "Spawn Animation Time")
	float m_SpawnAnimationTime{2.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Treasure Chest Settings", DisplayName = "Chest Open Animation Time")
	float m_ChestOpenAnimationTime{1.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Treasure Chest Settings", DisplayName = "Rotation Amount")
	float m_RotationAmount{360.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Treasure Chest Settings", DisplayName = "Translation Amount")
	float m_TranslationAmount{100.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "decomposition", DisplayName = "Decomposition curve")
	UCurveFloat* m_pDecompositionCurve;
	

private:
	float m_Decomposition{};
	float m_ElapsedAnimationTime{};
	bool m_bCompletedAnimationSpawnAnimation{false};
	bool m_bCompletedChestOpenSpawnAnimation{false};
	bool m_bStartOpenAnimation{false};
	bool m_bDecomposing{false};

	int m_NumberOfItemsSpawned{};

	UMaterialInterface* m_pMaterial;
	UMaterialInstanceDynamic* m_pDynamicMaterial;
	UStaticMeshComponent* m_pChestMesh{};
	FTimerHandle m_SpawnTimerHandle{};

	FTimeline m_DecompositionTimeLine;

	UFUNCTION()
	void DecomposeUpdate(float DeltaTime);
	UFUNCTION()
	void DecomposeFinished();

	void EnableSimulationMovementAndCollision(const bool enable) const;
	void PutChestInTheGround();
	void HandleSpawnAnimation(const float deltaTime);
};
