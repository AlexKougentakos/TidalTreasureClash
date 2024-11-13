// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshPointGetter.h"

#include "Gp27_TTC/ImGuiDebugger.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMeshPointGetter::AMeshPointGetter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMeshPointGetter::BeginPlay()
{	
	Super::BeginPlay();
	
	m_pMeshWithPoints = FindComponentByClass<UStaticMeshComponent>();

	
	if (!m_pMeshWithPoints)
	{
		UE_LOG(LogTemp, Error, TEXT("No mesh component found!"));
		return;
	}
	
	UImGuiDebugger* MyGameInstance = Cast<UImGuiDebugger>(GetWorld()->GetGameInstance());
	if (MyGameInstance != nullptr)
	{
		MyGameInstance->AddButton("Generate Points", [this]() { SelectPoints(m_NumberOfPoints); });
		MyGameInstance->AddNumericVariable("Number of Points", &m_NumberOfPoints, ENumericType::Int);
	}
	
}


void AMeshPointGetter::SetWaterLevel(AWaterLevel* waterLevel)
{
	m_pWaterLevel = waterLevel;
}

TArray<FVector> AMeshPointGetter::SelectPoints(uint32 numOfPoints)
{
	const FStaticMeshLODResources& LODResources = m_pMeshWithPoints->GetStaticMesh()->GetLODForExport(0); // LOD 0
	const FPositionVertexBuffer& vertexBuffer = LODResources.VertexBuffers.PositionVertexBuffer;
	const auto& vertexNormalsBuffer = LODResources.VertexBuffers.StaticMeshVertexBuffer;
	
	if (numOfPoints > vertexBuffer.GetNumVertices())
		numOfPoints = vertexBuffer.GetNumVertices();

	//Remove points from previous frame
	FlushPersistentDebugLines(GetWorld());
	
	for (uint32 i = 0; i < vertexBuffer.GetNumVertices(); i++)
	{
		FVector3f vertexPosition = vertexBuffer.VertexPosition(i);
		
		if (!GetWorld())
			UE_LOG(LogTemp, Error, TEXT("No world context found!"));
	}

	TArray<FVector> points{};
	GetPoints(vertexBuffer, vertexNormalsBuffer, numOfPoints, points);

	FCollisionQueryParams traceParameters = FCollisionQueryParams(FName(TEXT("lightning point")), true);
	traceParameters.bTraceComplex = true;
	traceParameters.bReturnPhysicalMaterial = false;

	TArray<FVector> outPoints{};
	FHitResult lightningPoint{};
	const FVector actorOffset{GetActorLocation()};
	for (const FVector& point : points)
	{
		//DrawDebugSphere(GetWorld(), point, 50.0f, 4, FColor::Red, true, -1, 1, 4);

		if(GetWorld()->LineTraceSingleByObjectType(lightningPoint,  point + actorOffset + FVector::UpVector * 1000.f,point + actorOffset -(FVector::DownVector * 20), ECC_WorldStatic, traceParameters))
		{
			if(lightningPoint.Location.Z > m_pWaterLevel->GetCurrentWaterLevel())
			{
				outPoints.Add(lightningPoint.Location);				
			}
		}		
	}
	return outPoints;
}

void AMeshPointGetter::GetPoints(const FPositionVertexBuffer& vertexBuffer, const FStaticMeshVertexBuffer& vertexNormalBuffer, uint32 numberOfPoints, TArray<FVector>& outPoints) const
{
	if (numberOfPoints > vertexBuffer.GetNumVertices())
		numberOfPoints = vertexBuffer.GetNumVertices();

	// Generate a list of indices [0, 1, ..., GetNumVertices() - 1]
	TArray<uint32> indices;
	indices.Reserve(vertexBuffer.GetNumVertices());
	for (uint32 i = 0; i < vertexBuffer.GetNumVertices(); i++)
	{
		indices.Add(i);
	}

	// Randomly shuffle the first 'numberOfPoints' indices to ensure uniqueness
	for (uint32 i = 0; i < numberOfPoints; ++i)
	{
		const int32 swapIndex = FMath::RandRange(i, indices.Num() - 1);
		indices.Swap(i, swapIndex);
	}

	// Use the first 'numberOfPoints' of the shuffled indices to pick unique points
	outPoints.Reserve(numberOfPoints); // Ensure there's enough space reserved
	for (uint32 pointsAdded = 0; pointsAdded < numberOfPoints;)
	{
		FVector3f vertexPosition = vertexBuffer.VertexPosition(indices[pointsAdded]);
		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Vertex Normal: %s"), *vertexNormalBuffer.VertexTangentZ(indices[pointsAdded]).ToString()));

		//todo: figure out how to draw the normal
		//Draw normal as a line
		DrawDebugLine(GetWorld(), FVector(vertexPosition), FVector(vertexPosition + vertexNormalBuffer.VertexTangentZ(indices[pointsAdded]) * 10.0f), FColor::Green, true, -1, 0, 4);

		pointsAdded++;
		outPoints.Add(FVector(vertexPosition));
	}
}

// Called every frame
void AMeshPointGetter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

