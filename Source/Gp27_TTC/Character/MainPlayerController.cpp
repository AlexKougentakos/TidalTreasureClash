// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"

#include <Gp27_TTC/Cannon/Cannon.h>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void AMainPlayerController::OnPossess(APawn* aPawn)
{
	// Call the parent method, to let it do anything it needs to
	Super::OnPossess(aPawn);

	// Store a reference to the Player's Pawn
	m_pPlayerCharacter = Cast<AMainCharacter>(aPawn);
	checkf(m_pPlayerCharacter,
	       TEXT("AMainPlayerController derived classes should only posess ACharacterBBBase derived pawns"));

	// Get a reference to the EnhancedInputComponent
	m_pEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(m_pEnhancedInputComponent,
	       TEXT("Unable to get reference to the EnhancedInputComponent."));

	
	m_pInputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		
	checkf(m_pInputSubsystem,
		TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem."));

	if(m_pActionFirePressed)
		m_pEnhancedInputComponent->BindAction(m_pActionFirePressed, ETriggerEvent::Started, this,
										   &AMainPlayerController::HandleCannonFire);
	if(m_pActionTurn)
		m_pEnhancedInputComponent->BindAction(m_pActionTurn, ETriggerEvent::Triggered, this,
										   &AMainPlayerController::HandleCannonTurn);
	if(m_pActionReleasePressed)
		m_pEnhancedInputComponent->BindAction(m_pActionReleasePressed, ETriggerEvent::Triggered, this,
										   &AMainPlayerController::HandleCannonRelease);
	// Bind the input actions.
	// Only attempt to bind if valid values were provided.
	if (m_pActionMove)
		m_pEnhancedInputComponent->BindAction(m_pActionMove, ETriggerEvent::Triggered, this,
										   &AMainPlayerController::HandleMove);

	if (m_pActionJump)
		m_pEnhancedInputComponent->BindAction(m_pActionJump, ETriggerEvent::Started, this,
										   &AMainPlayerController::HandleJump);
	
	if (m_pActionInteract)
		m_pEnhancedInputComponent->BindAction(m_pActionInteract, ETriggerEvent::Started, this,
										   &AMainPlayerController::HandleInteract);

	if (m_pActionInteractPressed)
		m_pEnhancedInputComponent->BindAction(m_pActionInteractPressed, ETriggerEvent::Started, this,
										   &AMainPlayerController::HandleInteractHeld);

	if (m_pActionInteractPressed)
		m_pEnhancedInputComponent->BindAction(m_pActionInteractPressed, ETriggerEvent::Completed, this,
										   &AMainPlayerController::HandleInteractReleased);

	if (m_pActionAttack)
		m_pEnhancedInputComponent->BindAction(m_pActionAttack, ETriggerEvent::Started, this,
										   &AMainPlayerController::HandleAttack);
	if (m_pActionPickup)
		m_pEnhancedInputComponent->BindAction(m_pActionPickup, ETriggerEvent::Started, this,
										   &AMainPlayerController::HandlePickup);
	SetInputToCharacterMovement();		
}

void AMainPlayerController::OnUnPossess()
{
	m_pEnhancedInputComponent->ClearActionBindings();	
	Super::OnUnPossess();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Bind function expects a non-const function
void AMainPlayerController::HandleMove(const FInputActionValue& inputActionValue)
{
	if (!m_bIsInputEnabled) return;
	
	//Get the player we are controlling
	if (m_pPlayerCharacter->IsDigging()) return;
	
	const FVector2D movementVector =  inputActionValue.Get<FVector2D>();

	// Forward is in the -Y and Right is in the +X direction.
	FVector worldMovementVector = FVector(movementVector.X, movementVector.Y * - 1.f, 0.0f);
	worldMovementVector.Normalize();

	// Move the player character in the world direction.
	m_pPlayerCharacter->AddMovementInput(worldMovementVector, 1.0f);

	// Calculate the target angle for the player. atan2 returns radians, so convert to degrees.
	const float targetAngleDegrees = FMath::RadiansToDegrees(atan2(worldMovementVector.X, -worldMovementVector.Y)) + 180.0f;
        
	// Get the current rotation of the player
	const float currentYaw = m_pPlayerCharacter->GetActorRotation().Yaw;

	// Calculate the shortest difference between the current yaw and the target angle
	const float deltaYaw = FMath::FindDeltaAngleDegrees(currentYaw, targetAngleDegrees);
	
	// Determine the amount to rotate this frame, clamping the maximum turn rate
	constexpr float maxTurnRate = 180.0f; // Maximum turn rate in degrees per second
	const float deltaTime = GetWorld()->DeltaTimeSeconds;
	const float turnAmount = FMath::Clamp(deltaYaw, -maxTurnRate * deltaTime, maxTurnRate * deltaTime);

	// Apply the rotation
	m_pPlayerCharacter->AddControllerYawInput(turnAmount);
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Bind function expects a non-const function
void AMainPlayerController::HandleJump()
{
	if (m_pPlayerCharacter->IsDigging()) return;

	if (!m_bIsInputEnabled) return;
	
	m_pPlayerCharacter->Jump();
}
// ReSharper disable once CppMemberFunctionMayBeConst
// Bind function expects a non-const function
void AMainPlayerController::HandleInteract()
{
	if (!m_bIsInputEnabled) return;
	m_pPlayerCharacter->InteractPressed();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Bind function expects a non-const function
void AMainPlayerController::HandleInteractHeld()
{
	if (!m_bIsInputEnabled) return;
	m_pPlayerCharacter->InteractHeld();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Bind function expects a non-const function
void AMainPlayerController::HandleInteractReleased()
{
	if (!m_bIsInputEnabled) return;
	m_pPlayerCharacter->InteractReleased();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Bind function expects a non-const function
void AMainPlayerController::HandleAttack()
{
	if (!m_bIsInputEnabled) return;
	if(m_pPlayerCharacter->GetIsAttacking()) return;
	
	m_pPlayerCharacter->SetIsAttacking();
}

// ReSharper disable once CppMemberFunctionMayBeConst
// Bind function expects a non-const function
void AMainPlayerController::HandlePickup()
{
	if (!m_bIsInputEnabled) return;
	if (m_pPlayerCharacter->IsDigging()) return;
	
	m_pPlayerCharacter->HandlePickUp();
	m_AmountOfPickups++;
}

void AMainPlayerController::HandleCannonFire()
{
	if (!m_bIsInputEnabled) return;
	m_pPlayerCharacter->FireCannon();
}

void AMainPlayerController::HandleCannonTurn(const FInputActionValue& inputActionValue)
{
	if (!m_bIsInputEnabled) return;
	const FVector2D targetMovement =  inputActionValue.Get<FVector2D>();
	m_pPlayerCharacter->MoveCannon(targetMovement);
}

void AMainPlayerController::HandleCannonRelease()
{
	if (!m_bIsInputEnabled) return;
	SetInputToCharacterMovement();
	m_pPlayerCharacter->ReleaseCannon();	
}

void AMainPlayerController::SetInputToCannon()
{
	if (!m_bIsInputEnabled) return;
	checkf(m_pInputMappingContentCannon, TEXT("InputMappingContentCannon was not specified."));
	m_pInputSubsystem->ClearAllMappings();
	m_pInputSubsystem->AddMappingContext(m_pInputMappingContentCannon, 0);	
	
}

void AMainPlayerController::SetInputToCharacterMovement()
{
	checkf(m_pInputMappingContent, TEXT("InputMappingContent was not specified."));
	m_pInputSubsystem->ClearAllMappings();
	m_pInputSubsystem->AddMappingContext(m_pInputMappingContent, 0);

	

}
