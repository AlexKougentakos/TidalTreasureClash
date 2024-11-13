// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CustomAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GP27_TTC_API UCustomAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsAttacking{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsCarrying{false};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsDigging{false};	
};
