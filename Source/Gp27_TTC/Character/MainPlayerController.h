// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
// Forward Declarations
class UEnhancedInputComponent;
class AMainCharacter;
class UInputMappingContext;

UCLASS(Abstract)
class AMainPlayerController : public APlayerController
{
public:
	// The Input Action to map to movement.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement", DisplayName="Move Action")
	UInputAction* m_pActionMove{};

	// The Input Action to map to jumping.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement", DisplayName="Jump Action")
	UInputAction* m_pActionJump{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement", DisplayName="Interact Action")
	UInputAction* m_pActionInteract{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement", DisplayName="Interact Pressed Action")
	UInputAction* m_pActionInteractPressed{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement", DisplayName="Attack Action")
	UInputAction* m_pActionAttack{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement", DisplayName="Pickup Action")
	UInputAction* m_pActionPickup{};

	//cannon input actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Cannon", DisplayName="Fire Pressed Action")
	UInputAction* m_pActionFirePressed{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Cannon", DisplayName="Release Action")
	UInputAction* m_pActionReleasePressed{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Cannon", DisplayName="Turn Action")
	UInputAction* m_pActionTurn{};
	
	// The Input Mapping Context to use.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	UInputMappingContext* m_pInputMappingContent{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Cannon")
	UInputMappingContext* m_pInputMappingContentCannon{};

	void SetInputToCannon();
	void SetInputToCharacterMovement();
	void SetInputEnabled(const bool bIsEnabled) {m_bIsInputEnabled = bIsEnabled;}
protected:
	// Action Handler Functions
	void HandleMove(const FInputActionValue& inputActionValue);
	void HandleJump();
	void HandleInteract();
	void HandleInteractHeld();
	void HandleInteractReleased();
	void HandleAttack();
	void HandlePickup();
	void HandleCannonFire();
	void HandleCannonTurn(const FInputActionValue& inputActionValue);
	void HandleCannonRelease();

	
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

private:
	// Used to store a reference to the InputComponent cast to an EnhancedInputComponent.
	UPROPERTY()
	UEnhancedInputComponent* m_pEnhancedInputComponent{};
	UEnhancedInputLocalPlayerSubsystem* m_pInputSubsystem{};
	// Used to store a reference to the pawn we are controlling.
	UPROPERTY()
	AMainCharacter* m_pPlayerCharacter{};

	FTimerHandle m_TimerHandle;
	float m_TimeToTrace{1};
	
	
	int m_AmountOfPickups{};
	bool m_bIsInputEnabled{true};
	
	GENERATED_BODY()
};
