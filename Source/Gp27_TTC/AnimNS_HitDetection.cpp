// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNS_HitDetection.h"
#include "Character/MainCharacter.h"

void UAnimNS_HitDetection::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	AMainCharacter* mainCharacter = Cast<AMainCharacter>(MeshComp->GetOwner());
	if(IsValid(mainCharacter))
	{
		mainCharacter->TraceAttack();
	}
}
