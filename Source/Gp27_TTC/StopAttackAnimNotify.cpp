// Fill out your copyright notice in the Description page of Project Settings.


#include "StopAttackAnimNotify.h"

#include "Character/MainCharacter.h"

void UStopAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(MeshComp->GetOwner());
	if(IsValid(mainCharacter))
	{
		mainCharacter->SetIsAttacking();
		mainCharacter->SetHasAttackHit(false);
	}

}
