#include "DigSpotSpawner.h"

#include "DigSpot.h"
#include "Gp27_TTC/ImGuiDebugger.h"
#include "Kismet/GameplayStatics.h"


ADigSpotSpawner::ADigSpotSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ADigSpotSpawner::BeginPlay()
{
	Super::BeginPlay();
	m_CurrentDelayBetweenSpawns = FMath::RandRange(m_MinimumDelayBetweenSpawns, m_MaximumDelayBetweenSpawns);

	GetDigSpotLocations();
}

void ADigSpotSpawner::Tick(float deltaTime)
{
	Super::Tick(deltaTime);
	m_TimeSinceLastSpawn += deltaTime;

	if (CanSpawnTreasure())
	{
		SpawnTreasure();
		m_TimeSinceLastSpawn = 0.0f;
		m_bWasPreviousTreasureOpened = false;
		m_CurrentDelayBetweenSpawns = FMath::RandRange(m_MinimumDelayBetweenSpawns, m_MaximumDelayBetweenSpawns);
	}
}

void ADigSpotSpawner::OnTreasureOpened()
{
	m_bWasPreviousTreasureOpened = true;
}

void ADigSpotSpawner::SpawnTreasure()
{
	const FVector spawnLocation = SelectSpawningLocation();
	const FVector spawnLocationWithOffset = FVector{spawnLocation.X, spawnLocation.Y, spawnLocation.Z + 50.0f};
	//Offset to avoid projection issues with the decal
	
	const auto spawnedActor = GetWorld()->SpawnActor(m_pDigSpotClass, &spawnLocationWithOffset);
	const auto digSpot = Cast<ADigSpot>(spawnedActor);
	digSpot->OnInteractionCompleted.AddDynamic(this, &ADigSpotSpawner::OnTreasureOpened);
}

void ADigSpotSpawner::GetDigSpotLocations()
{
	//Get all dig spots
	const TArray<AActor*> foundDigSpots = [&]() -> TArray<AActor*>
	{
		TArray<AActor*> _foundDigSpots;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), m_pDigSpotLocationClass, _foundDigSpots);
		return _foundDigSpots;
	}();

	//Assign the locations to the array
	//I make another array since we only need the locations and not the actors themselves
	for (const auto& foundDigSpot : foundDigSpots)
	{
		//Cast a ray down to find the ground
		FHitResult hitResult{};
		const FVector start = foundDigSpot->GetActorLocation();
		const FVector end = start - FVector::UpVector * 1000.0f;
		//TODO: Replace ECC_WorldStatic with a custom channel for the ground to avoid hitting other actors
		GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_WorldStatic);

		checkf(hitResult.bBlockingHit, TEXT("No ground found for dig spot!"));
		m_DigSpots.Push(hitResult.ImpactPoint);
	}
}

void ADigSpotSpawner::RemovePointsUnderTheWater()
{
	const float currentWaterY = m_pWaterPlane->GetActorLocation().Z;

	TArray<FVector> digSpotsToKeep{};
	
	for (const auto& potentialSpot : m_DigSpots)
	{
		if (currentWaterY < potentialSpot.Z)
			digSpotsToKeep.Push(potentialSpot);
	}

	m_DigSpots = digSpotsToKeep;
}

FVector ADigSpotSpawner::SelectSpawningLocation()
{
	RemovePointsUnderTheWater();

	//Select random point from the remaining array
	const int32 randomIndex = FMath::RandRange(0, m_DigSpots.Num() - 1);
	return m_DigSpots[randomIndex];
}

bool ADigSpotSpawner::CanSpawnTreasure() const
{
	const bool bHasEnoughTimePassed = m_TimeSinceLastSpawn >= m_CurrentDelayBetweenSpawns;

	if (m_bDontSpawnUntilOpened)
		return bHasEnoughTimePassed && m_bWasPreviousTreasureOpened;

	return bHasEnoughTimePassed;
}
