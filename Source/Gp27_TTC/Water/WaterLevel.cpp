// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterLevel.h"

#include "IWaterInteractable.h"
#include "NiagaraFunctionLibrary.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Gp27_TTC/Ship/Ship.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWaterLevel::AWaterLevel()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AWaterLevel::BeginPlay()
{
	Super::BeginPlay();
	m_pMeshComponent = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	m_MaximumStage = m_WaterLevelStages.Num();

	checkf(m_pShip1, TEXT("ship 1 not valid"));
	checkf(m_pShip2, TEXT("ship 2 not valid"));
	
	m_ZDifferenceShip1 = m_pShip1->GetActorLocation().Z - GetActorLocation().Z;
	m_ZDifferenceShip2 = m_pShip2->GetActorLocation().Z - GetActorLocation().Z;

	m_StartingWaterLevel = GetActorLocation().Z;

	m_pMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCollisionWithActors);
}

// Called every frame
void AWaterLevel::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
	if(m_bIsWaterLevelRising)
	{
		UpdatePosition(DeltaTime);
	}
	CheckDrowning();
}

void AWaterLevel::StartRisingWaterLevel()
{
	if(m_CurrentStage < m_MaximumStage)
	{
		m_bIsWaterLevelRising = true;
		
		//Apply camera shake
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PlayerController->ClientStartCameraShake(m_pCameraShake);
		m_WaterRisingSoundVolume += 0.15f;
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pWaterRisingSound, GetActorLocation(), m_WaterRisingSoundVolume);
	}
}

void AWaterLevel::GetPlayers()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainCharacter::StaticClass(), m_pCharacterActors);
}

float AWaterLevel::GetCurrentWaterLevel()
{	
	return GetActorLocation().Z;
}

bool AWaterLevel::PassedHalfStage() const
{
	return (m_CurrentStage >= m_MaximumStage/2);
}

void AWaterLevel::UpdatePosition(float DeltaTime)
{
	
	if(!m_bIsWaterLevelRising) return;
	
	const FVector worldLocation = GetActorLocation();
	const FVector ship1WorldLocation = m_pShip1->GetActorLocation();
	const FVector ship2WorldLocation = m_pShip2->GetActorLocation();

	float ship1DesiredZ, ship2DesiredZ;

	// Calculate water progress and desired Z locations based on stages up to the second to last stage
	if (m_CurrentStage < m_WaterLevelStages.Num() - 1)
	{
	    const float waterProgress = (m_WaterLevelStages[m_CurrentStage] - m_StartingWaterLevel)
		/ (m_WaterLevelStages[ m_WaterLevelStages.Num() - 2] - m_StartingWaterLevel);
		
	    ship1DesiredZ = FMath::Lerp(m_MinShipZPosition, m_MaxShipZPosition, waterProgress);
	    ship2DesiredZ = FMath::Lerp(m_MinShipZPosition, m_MaxShipZPosition, waterProgress);
	}
	else
	{
	    // Continue moving up with the water level in the final stage
	    ship1DesiredZ = ship1WorldLocation.Z + m_WaterMaxSpeed * DeltaTime;
	    ship2DesiredZ = ship2WorldLocation.Z + m_WaterMaxSpeed * DeltaTime;
	}

	// Desired locations for water and ships
	FVector desiredLocation = FMath::Lerp(worldLocation, FVector{worldLocation.X, worldLocation.Y, m_WaterLevelStages[m_CurrentStage]}, 0.1f);
	FVector ship1DesiredLocation = FVector{ship1WorldLocation.X, ship1WorldLocation.Y, ship1DesiredZ};
	FVector ship2DesiredLocation = FVector{ship2WorldLocation.X, ship2WorldLocation.Y, ship2DesiredZ};

	const FVector direction = FVector::UpVector;
	const float distance = FVector::Dist(worldLocation, desiredLocation);

	if (distance > m_WaterMaxSpeed * DeltaTime)
	{
	    desiredLocation = worldLocation + direction * m_WaterMaxSpeed * DeltaTime;

	    // Update ship locations based on water progress up to the second to last stage or continue moving up in the last stage
	    if (m_CurrentStage < m_WaterLevelStages.Num() - 1)
	    {
	        ship1DesiredLocation = FVector{ship1WorldLocation.X, ship1WorldLocation.Y, FMath::Lerp(m_MinShipZPosition, m_MaxShipZPosition, (desiredLocation.Z - m_StartingWaterLevel) / (m_WaterLevelStages[m_WaterLevelStages.Num() - 2] - m_StartingWaterLevel))};
	        ship2DesiredLocation = FVector{ship2WorldLocation.X, ship2WorldLocation.Y, FMath::Lerp(m_MinShipZPosition, m_MaxShipZPosition, (desiredLocation.Z - m_StartingWaterLevel) / (m_WaterLevelStages[m_WaterLevelStages.Num() - 2] - m_StartingWaterLevel))};
	    }
		else
	    {
	        const float additionalHeight = m_WaterMaxSpeed * DeltaTime;
	        ship1DesiredLocation.Z = ship1WorldLocation.Z + additionalHeight;
	        ship2DesiredLocation.Z = ship2WorldLocation.Z + additionalHeight;
	    }
	}

	SetActorLocation(desiredLocation);
	m_pShip1->SetActorLocation(ship1DesiredLocation);
	m_pShip2->SetActorLocation(ship2DesiredLocation);
	
	if(GetActorLocation().Z + 1.f >= m_WaterLevelStages[m_CurrentStage])
	{
		m_bIsWaterLevelRising = false;
		m_CurrentStage++;

		//Get all actors of type IWaterInteractable
		TArray<AActor*> pInteractables;
		UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UWaterInteractable::StaticClass(), pInteractables);
		for (const auto pInteractable : pInteractables)
		{
			const auto pWaterInteractable = Cast<IWaterInteractable>(pInteractable);
			
			if (pWaterInteractable->DeleteWhenInWater()
				&& pInteractable->GetActorLocation().Z < GetActorLocation().Z)
			{
				pWaterInteractable->OnDeletion();
				pWaterInteractable->SetIsDecomposing(true);
			}
		}
		
		if (m_CurrentStage == m_MaximumStage)
		{
			OnFinalWaterLevelReached.Broadcast();
		}
	}
}

void AWaterLevel::CheckDrowning()
{
	if(m_pCharacterActors.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("no character refs")));
	}
	for (const auto pCharacter : m_pCharacterActors)
	{
		AMainCharacter* tempCharacter = Cast<AMainCharacter>(pCharacter);
		if(tempCharacter->GetActorLocation().Z < GetActorLocation().Z - m_DrowningDifference)
		{
			tempCharacter->DoDamageToSelf(50);
		}
	}
}


void AWaterLevel::OnCollisionWithActors(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (!OtherActor || !OtherActor->GetClass()->ImplementsInterface(UWaterInteractable::StaticClass())) return;

	IWaterInteractable* pInteractable = Cast<IWaterInteractable>(OtherActor);

	if (!pInteractable) return;
	
	if (pInteractable->DeleteWhenInWater())
	{
		pInteractable->OnDeletion();
		pInteractable->SetIsDecomposing(true);
	}

	if (pInteractable->DoSplash())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			 GetWorld(),
			 m_pWaterSplashNiagaraSystem,
			 OtherActor->GetActorLocation(),
			 FRotator::ZeroRotator,
			 FVector{pInteractable->GetSplashScale()}
			 );
	}
}

