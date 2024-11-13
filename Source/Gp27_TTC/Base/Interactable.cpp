#include "Interactable.h"


AInteractable::AInteractable()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

