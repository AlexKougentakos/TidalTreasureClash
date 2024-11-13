#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteraction, AInteractable*, Interactable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractioCompleted);

enum class InteractableType
{
	PRESSED,
	HELD
};

/// Inherit from this class to create an interactable object
UCLASS()
class GP27_TTC_API AInteractable : public AActor
{
	GENERATED_BODY()

public:
	AInteractable();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact() {};
	virtual void InteractStop() {}; //To be used with InteractableType::HELD
	
	FOnInteraction OnInteractionStarted;
	FOnInteractioCompleted OnInteractionCompleted;

	InteractableType GetInteractableType() const { return m_InteractableType; }
	
protected:
	InteractableType m_InteractableType;
};