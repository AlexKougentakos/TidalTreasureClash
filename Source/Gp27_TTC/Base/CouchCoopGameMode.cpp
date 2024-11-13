// Fill out your copyright notice in the Description page of Project Settings.


#include "CouchCoopGameMode.h"

#include "EndGameState.h"
#include "EngineUtils.h"
#include "MainGameState.h"
#include "MovingCamera.h"
#include "GameFramework/PlayerStart.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Gp27_TTC/Character/MainPlayerController.h"
#include "Gp27_TTC/Character/PlayerSpawn.h"
#include "Gp27_TTC/Treasure/DigSpotSpawner.h"
#include "Gp27_TTC/Water/WaterLevel.h"
#include "Kismet/GameplayStatics.h"

ACouchCoopGameMode::ACouchCoopGameMode()
{
	
}

void ACouchCoopGameMode::SetEndGameState(FTeamStats leftTeamStats, FTeamStats rightTeamStats) const
{
	AEndGameState* pEndGameState = GetWorld()->SpawnActor<AEndGameState>(m_pEndGameState);

	checkf(pEndGameState, TEXT("Failed to spawn end game state"));
	
	pEndGameState->SetTeamStats(leftTeamStats, rightTeamStats);
	GetWorld()->SetGameState(pEndGameState);

	//Disable gameplay events
	m_pLightningManager->SetLightningEnabled(false);
}

void ACouchCoopGameMode::SetMainGameState() const
{
	const auto pMainGameState = GetWorld()->SpawnActor<AMainGameState>(m_pMainGameState);
	
	pMainGameState->GetPlayers();
	GetWorld()->SetGameState(pMainGameState);
	m_pLightningManager->SetLightningEnabled(true);
	m_pDigSpotSpawner->SetSpawningEnabled(true);


	//Enable Input
	for (int i{}; i < m_pPlayers.Num(); i++)
	{
		const auto playerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),i));
		playerController->SetInputEnabled(true);
	}

}

void ACouchCoopGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> pWaterLevelActors{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWaterLevel::StaticClass(), pWaterLevelActors);
	m_pWaterLevel = Cast<AWaterLevel>(pWaterLevelActors[0]);
	checkf(m_pWaterLevel ,TEXT("no water level in scene"));

	m_pLightningManager = Cast<ALightningManager>( UGameplayStatics::GetActorOfClass(GetWorld(), ALightningManager::StaticClass()));
	checkf(m_pLightningManager, TEXT("No lightning manager in scene"));

	m_pDigSpotSpawner = Cast<ADigSpotSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ADigSpotSpawner::StaticClass()));
	checkf(m_pDigSpotSpawner, TEXT("No dig spot spawner in scene"));
	
	SpawnPlayers();
	SetMainCamera();

	//Disable Input
	for (int i{}; i < m_pPlayers.Num(); i++)
	{
		const auto playerController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),i));
		playerController->SetInputEnabled(false);
	}
}

void ACouchCoopGameMode::SpawnPlayers()
{
	//Get all player starts
	TArray<AActor*> foundPlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerSpawn::StaticClass(), foundPlayerStarts);
	
	for (const auto& playerStart : foundPlayerStarts)
	{
		if (auto* playerStartCasted = Cast<APlayerSpawn>(playerStart))
			m_pPlayerStarts.Push(playerStartCasted);		
	}

	bool bAssignedBlueHat = false;
	bool assignedRedHat = false;
	//Create players
	for (uint8 i = 0; i < m_pPlayerStarts.Num(); i++)
	{
		checkf(GetWorld(), TEXT("No world context found!"));

		const auto tag = m_pPlayerStarts[i]->GetPlayerSpawnTag();
		
		//const auto mainPlayerController = Cast<AMainPlayerController>(UGameplayStatics::CreatePlayer(GetWorld(), i, true));
		const auto mainPlayerController = UGameplayStatics::CreatePlayer(GetWorld(), i, true);

		const FVector spawnLocation = m_pPlayerStarts[i]->GetActorTransform().GetLocation();
		const auto playerCreated = Cast<AMainCharacter>(GetWorld()->SpawnActor(m_pMainCharacterClass, &spawnLocation));
		checkf(playerCreated, TEXT("Failed to spawn player %d"), i);

		//Get player mesh component
		const auto pPlayerMeshComponent = playerCreated->GetMesh();
		UMaterialInstanceDynamic* pPlayerMaterialInstance = Cast<UMaterialInstanceDynamic>(pPlayerMeshComponent->GetMaterial(0));

		const auto pPlayerIndicatorMeshComponent = Cast<UStaticMeshComponent>(playerCreated->GetComponentsByTag(UStaticMeshComponent::StaticClass(),"Indicator")[0]);
		UMaterialInstanceDynamic* pIndicatorMaterialInstance = Cast<UMaterialInstanceDynamic>(pPlayerIndicatorMeshComponent->GetMaterial(0));

		
		if (!pPlayerMaterialInstance)
		{
			pPlayerMaterialInstance = UMaterialInstanceDynamic::Create(pPlayerMeshComponent->GetMaterial(0), pPlayerMeshComponent);
			pPlayerMeshComponent->SetMaterial(0, pPlayerMaterialInstance);
		}

		if(!pIndicatorMaterialInstance)
		{
			pIndicatorMaterialInstance = UMaterialInstanceDynamic::Create(pPlayerIndicatorMeshComponent->GetMaterial(0), pPlayerIndicatorMeshComponent);
			pPlayerIndicatorMeshComponent->SetMaterial(0, pIndicatorMaterialInstance);
		}
		
		const FName parameterName = TEXT("Base Color Overlay");
		const FLinearColor teamColor= [&]()->FLinearColor
		{
			if (tag == "Blue")
				return m_BlueTeamColor;
			if (tag == "Red")
				return m_RedTeamColor;

			return {};
		}();

		
		
		if (tag == "Blue")
		{
			if(!bAssignedBlueHat)
			{
				pPlayerMeshComponent->SetSkeletalMesh(m_pModelWithHat);
				bAssignedBlueHat = true;
			}
			
			playerCreated->SetTeamID(1); //This is fixed and should be the same in the editor
			pPlayerIndicatorMeshComponent->SetCustomDepthStencilValue(2);
		}
		
		if (tag == "Red")
		{
			if (!assignedRedHat)
			{
				pPlayerMeshComponent->SetSkeletalMesh(m_pModelWithHat);
				assignedRedHat = true;
			}
			
			playerCreated->SetTeamID(0); //This is fixed and should be the same in the editor
			pPlayerIndicatorMeshComponent->SetCustomDepthStencilValue(3);
		} 
		
		pPlayerMaterialInstance->SetVectorParameterValue(parameterName, teamColor);
		pIndicatorMaterialInstance->SetVectorParameterValue(TEXT("TeamColor"), teamColor);
		
		if (i > 0) //We are not player 1
		{
			m_pPlayers.Push(playerCreated);
			
			playerCreated->SetRespawnLocation(m_pPlayerStarts[i]->GetTransform());
			
			//mainPlayerController->Possess(playerCreated);
			UGameplayStatics::GetPlayerController(GetWorld(), i)->Possess(playerCreated);
		}
		else //We are player 1
		{
			UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(playerCreated);

			m_pPlayers.Push(playerCreated);
			playerCreated->SetRespawnLocation(m_pPlayerStarts[i]->GetTransform());
		}
	}
	
	m_pWaterLevel->GetPlayers();
}

void ACouchCoopGameMode::SetMainCamera() const
{   
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
		return;

	// Iterate over all actors in the scene
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* actor = *It;
		if (actor && actor->IsA(m_pMovingCameraClass))
		{
			AMovingCamera* movingCamera = Cast<AMovingCamera>(actor);

			movingCamera->SetPlayers(m_pPlayers);
			PlayerController->SetViewTarget(movingCamera);
			return; // We only need 1 camera
		}
	}
}


