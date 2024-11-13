// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ship.generated.h"

class UBoxComponent;
class ATreasureChest;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShipLootScored, int32, teamID, ATreasureChest*, pTreasure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShipLootStolen, int32, teamID, ATreasureChest*, pTreasure);

UCLASS()
class GP27_TTC_API AShip : public AActor
{
	GENERATED_BODY()
	
public:
	AShip();	
	
	UPROPERTY(EditInstanceOnly, Category = "Ship", DisplayName = "Team ID")
	int m_TeamID{};
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Ship")
	FOnShipLootScored OnShipLootScored;
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "Ship")
	FOnShipLootStolen OnShipLootStolen;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UBoxComponent* m_pLootScoreArea{};

	AActor* m_pWaterLevel{};
	
	UFUNCTION() //Necessary to bind to the OnScoreAreaBeginOverlap function
	void OnScoreAreaBeginOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);
	UFUNCTION()
	void OnScoreAreaEndOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex);
	
	void InitializeColliderComponent();
public:

};
