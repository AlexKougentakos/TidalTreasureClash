#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerSpawn.generated.h"

UCLASS()
class GP27_TTC_API APlayerSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerSpawn();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	FString GetPlayerSpawnTag() const { return m_PlayerSpawnTag; }
private:
	UPROPERTY(EditAnywhere, Category = "Player Spawn", DisplayName = "Player Spawn Tag")
	FString m_PlayerSpawnTag{};

	
};
