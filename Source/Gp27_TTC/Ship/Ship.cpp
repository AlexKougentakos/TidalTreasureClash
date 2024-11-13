#include "Ship.h"

#include "Components/BoxComponent.h"
#include "Gp27_TTC/ImGuiDebugger.h"
#include "Gp27_TTC/Treasure/Treasure.h"


AShip::AShip()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShip::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeColliderComponent();
}

void AShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
// Binding expects specific function parameters
void AShip::OnScoreAreaBeginOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor,
                                    UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	if (pOtherActor->ActorHasTag("Treasure"))
		OnShipLootScored.Broadcast(m_TeamID);
}

// ReSharper disable once CppMemberFunctionMayBeConst
// ReSharper disable once CppParameterMayBeConstPtrOrRef
// Binding expects specific function parameters
void AShip::OnScoreAreaEndOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor,
                                  UPrimitiveComponent* pOtherComp, int32 otherBodyIndex)
{
	if(pOtherActor->ActorHasTag("Treasure"))
		OnShipLootStolen.Broadcast(m_TeamID);
}

void AShip::InitializeColliderComponent()
{
	//Tag given to the UBox Collider inside the ship that scores the loot
	const FName componentTag = {"LootScoreZone"};

	TArray<UBoxComponent*> boxComponents;
	GetComponents<UBoxComponent>(boxComponents);

	//Get the loot score area collider
	for (UBoxComponent* pBoxComponent : boxComponents)
	{
		if (!pBoxComponent->ComponentHasTag(componentTag)) continue;
		
		m_pLootScoreArea = pBoxComponent;
		break;
	}
	
	checkf(m_pLootScoreArea, TEXT("No Loot Score Area found in the Ship!"));

	m_pLootScoreArea->OnComponentBeginOverlap.AddDynamic(this, &AShip::OnScoreAreaBeginOverlap);
	m_pLootScoreArea->OnComponentEndOverlap.AddDynamic(this, &AShip::OnScoreAreaEndOverlap);
}
