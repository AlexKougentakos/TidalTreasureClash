// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gp27_TTC/Water/WaterLevel.h"
#include "MeshPointGetter.generated.h"

UCLASS()
class GP27_TTC_API AMeshPointGetter : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeshPointGetter();
	virtual void Tick(float DeltaTime) override;

	//Public variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Mesh Points")
	UStaticMeshComponent* m_pMeshWithPoints{};

	void SetWaterLevel(AWaterLevel* waterLevel);
	
	TArray<FVector> SelectPoints(uint32 numOfPoints);
protected:
	virtual void BeginPlay() override;
	
private:
	
	
	void GetPoints(const FPositionVertexBuffer& vertexBuffer, const FStaticMeshVertexBuffer& vertexNormalBuffer, uint32 numberOfPoints, TArray<FVector>& outPoints) const;

	uint32 m_NumberOfPoints{};

	AWaterLevel* m_pWaterLevel;
	
};
