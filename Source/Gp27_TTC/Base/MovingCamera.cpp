#include "MovingCamera.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gp27_TTC/ImGuiDebugger.h"
#include "Gp27_TTC/Character/MainCharacter.h"

AMovingCamera::AMovingCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMovingCamera::BeginPlay()
{
	Super::BeginPlay();

	m_StartingXLocation = GetActorLocation().X;
	m_StartingYLocation = GetActorLocation().Y;
	//print the starting x location
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Starting X Location: %f"), m_StartingXLocation));
	
	m_pVisualizationSphere = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	m_pSpringArmComponent = Cast<USpringArmComponent>(GetComponentByClass(USpringArmComponent::StaticClass()));
}

void AMovingCamera::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	UpdateCameraPosition(deltaTime);

	UpdateCameraZoom(deltaTime);
}

void AMovingCamera::SetPlayers(const TArray<AMainCharacter*>& players)
{
	m_pPlayersToTrack = players;

	for (const auto& player : players)
	{
		player->OnPlayerDeath.AddDynamic(this, &AMovingCamera::RemoveTrackedPlayer);
		player->OnPlayerRespawn.AddDynamic(this, &AMovingCamera::AddTrackedPlayer);
	}
}

FVector AMovingCamera::GetAverageLocationOfTrackedPlayers(const TArray<AMainCharacter*>& actorArray) const
{
	FVector totalLocation = FVector::ZeroVector;
	for (const AMainCharacter* mainCharacter : actorArray)
	{
		totalLocation += mainCharacter->GetActorLocation();
	}

	return totalLocation / actorArray.Num();
}

float AMovingCamera::GetLargestDistanceBetweenPlayers(const TArray<AMainCharacter*>& actorArray) const
{
	float maxDistance = 0.0f;

	// Iterate over all pairs of players in the array
	for (int32 i = 0; i < actorArray.Num(); i++)
	{
		for (int32 j = i + 1; j < actorArray.Num(); j++)
		{
			//We don't have to do this check since it will be 0 but save the calculation
			if (actorArray[i] == actorArray[j])
				continue;

			// Calculate the distance between the two players
			const float distance = FVector::Dist(actorArray[i]->GetActorLocation(), actorArray[j]->GetActorLocation());
            
			// Update MaxDistance if the current distance is greater
			if (distance > maxDistance)
				maxDistance = distance;
		}
	}

	return maxDistance;
}

float AMovingCamera::GetZoomPercent() const
{
	return (m_pSpringArmComponent->TargetArmLength - m_MinCameraDistance) / (m_MaxCameraDistance - m_MinCameraDistance);
}

void AMovingCamera::UpdateCameraPosition(float deltaTime) const
{
	//Handle moving the camera
	const FVector worldLocation = m_pVisualizationSphere->GetOwner()->GetActorLocation();
	const FVector averageLocation = GetAverageLocationOfTrackedPlayers(m_pPlayersToTrack);
	FVector desiredLocation = FMath::Lerp(worldLocation, FVector{averageLocation.X, averageLocation.Y, GetActorLocation().Z}, 0.1f);

	//Clamp X movement
	const float clampValue = FMath::Lerp(MoveOffsetRange.Y, MoveOffsetRange.X, GetZoomPercent());
	if (desiredLocation.X > m_StartingXLocation + clampValue) { desiredLocation.X = m_StartingXLocation + clampValue; }
	else if (desiredLocation.X < m_StartingXLocation - clampValue)
	{
		desiredLocation.X = m_StartingXLocation - clampValue;
	}

	//Clamp Y movement
	if (desiredLocation.Y > m_StartingYLocation + m_MaxMoveOffsetY)
	{
		desiredLocation.Y = m_StartingYLocation + m_MaxMoveOffsetY;
	}
	else if (desiredLocation.Y < m_StartingYLocation - m_MaxMoveOffsetY)
	{
		desiredLocation.Y = m_StartingYLocation - m_MaxMoveOffsetY;
	}
	
	// Cap the maximum speed of the camera movement
	const FVector direction = (desiredLocation - worldLocation).GetSafeNormal();
	const float distance = FVector::Dist(worldLocation, desiredLocation);
	if (distance > m_CameraMaxSpeed * deltaTime)
	{
		desiredLocation = worldLocation + direction * m_CameraMaxSpeed * deltaTime;
	}

	

	m_pVisualizationSphere->SetWorldLocation(desiredLocation);
}

void AMovingCamera::UpdateCameraZoom(float deltaTime) const
{
	//Handle zooming the camera
	const float currentTargetArmLength = m_pSpringArmComponent->TargetArmLength;
	const float desiredTargetArmLength = FMath::Clamp(GetLargestDistanceBetweenPlayers(m_pPlayersToTrack), m_MinCameraDistance, m_MaxCameraDistance);
	
	m_pSpringArmComponent->TargetArmLength = FMath::FInterpTo(currentTargetArmLength, desiredTargetArmLength, deltaTime, m_ZoomSpeed);
}

void AMovingCamera::AddTrackedPlayer(AMainCharacter* pPlayer)
{
	m_pPlayersToTrack.Push(pPlayer);
}

void AMovingCamera::RemoveTrackedPlayer(AMainCharacter* pPlayer)
{
	m_pPlayersToTrack.Remove(pPlayer);
}


