#include "Treasure.h"

ATreasure::ATreasure()
{
	PrimaryActorTick.bCanEverTick = false;

	//Add treasure tag
	Tags.Add("Treasure");
}

void ATreasure::BeginPlay()
{
	Super::BeginPlay();
}

void ATreasure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

