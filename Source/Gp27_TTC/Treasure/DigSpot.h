#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Gp27_TTC/Base/Interactable.h"
#include "Components/WidgetComponent.h"
#include "Gp27_TTC/Water/IWaterInteractable.h"
#include "DigSpot.generated.h"

class USphereComponent;
class UWidgetComponent;
class UCameraComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
UCLASS()
class GP27_TTC_API ADigSpot : public AInteractable, public IWaterInteractable
{
	GENERATED_BODY()
	
public:	
	ADigSpot();

	virtual void BeginPlay() override;
	void RotateInteractionPrompt();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Digging Settings", DisplayName = "Time To Dig")
	float m_TimeToDig{5.0f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Digging Settings", DisplayName = "Treasure Chest Class")
	UClass* m_pTreasureChestClass{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "decomposition", DisplayName = "Decomposition curve")
	UCurveFloat* m_pDecompositionCurve;


	virtual void OnDeletion() override;

	virtual void SetIsDecomposing(bool isDecomposing) override;
protected:
	virtual void Interact() override;
	virtual void InteractStop() override;
	

private:
	bool m_bInteractionStarted{false};
	bool m_bHasSpawnedTreasure{false};
	float m_CurrentDigTime{};
	bool m_bDecomposing{false};
	float m_Decomposition{};
	UWidgetComponent* m_pWidgetComponent{};
	UCameraComponent* m_pCameraComponent{};
	USphereComponent* m_pPickupRadiusSphereComponent{};
	UDecalComponent* m_pDecal;
	
	UFUNCTION()
	void DecomposeUpdate(float DeltaTime);
	UFUNCTION()
	void DecomposeFinished();
	
	UMaterialInterface* m_pMaterial;
	UMaterialInstanceDynamic* m_pDynamicMaterial;
	FTimeline m_DecompositionTimeLine;
	
};