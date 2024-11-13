#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingCamera.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AMainCharacter;

UCLASS()
class GP27_TTC_API AMovingCamera : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovingCamera();
	
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	
	void SetPlayers(const TArray<AMainCharacter*>& players);
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera Settings", DisplayName = "Max Camera Distance")
	float m_MaxCameraDistance{1700.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera Settings", DisplayName = "Min Camera Distance")
	float m_MinCameraDistance{700.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera Settings", DisplayName = "Max Camera Speed")
	float m_CameraMaxSpeed{1000.f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera Settings", DisplayName = "Zoom Speed")
	float m_ZoomSpeed{2.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera Settings", DisplayName = "Move Offset X Range",
		meta=(ToolTip="The minimum is for when the camera is fully zoomed out and the max for when it's fully zoomed in"))
	FVector2D MoveOffsetRange{150.f, 500.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera Settings", DisplayName = "Max Move Offset Y")
	float m_MaxMoveOffsetY{50.f};
	
	
private:
	UFUNCTION()
	void AddTrackedPlayer(AMainCharacter* pPlayer);
	UFUNCTION()
	void RemoveTrackedPlayer(AMainCharacter* pPlayer);

	void UpdateCameraPosition(float deltaTime) const;
	void UpdateCameraZoom(float deltaTime) const;
	FVector GetAverageLocationOfTrackedPlayers(const TArray<AMainCharacter*>& actorArray) const;
	float GetLargestDistanceBetweenPlayers(const TArray<AMainCharacter*>& actorArray) const;

	float GetZoomPercent() const;
	
	TArray<AMainCharacter*> m_pPlayersToTrack{};
	UStaticMeshComponent* m_pVisualizationSphere{};
	USpringArmComponent* m_pSpringArmComponent{};

	float m_StartingXLocation{};
	float m_StartingYLocation{};
};
