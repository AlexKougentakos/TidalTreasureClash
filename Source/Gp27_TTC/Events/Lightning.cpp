// Fill out your copyright notice in the Description page of Project Settings.


#include "Lightning.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/GarbageCollectionSchema.h"
#include "Camera/CameraShakeBase.h"

// Sets default values
ALightning::ALightning()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALightning::BeginPlay()
{
	Super::BeginPlay();
	m_LightningLifeTime = FMath::FRandRange(1.f,5.f);	
}

// Called every frame
void ALightning::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(m_bIsCounting)
	{
		m_LightningCounter += DeltaTime;
	}
	if(m_LightningCounter >= m_LightningLifeTime)
	{
		CheckStrike();

		//Apply camera shake
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PlayerController->ClientStartCameraShake(m_pCameraShake);
	}
}

void ALightning::SetNiagaraSystem(UNiagaraSystem* niagaraSystem)
{
	m_pNiagaraSystemStrike = niagaraSystem;
}

void ALightning::SetSound(USoundBase* sound)
{
	m_pLightingStrikeSFX = sound;
}

void ALightning::SetCameraShake(TSubclassOf<UCameraShakeBase> shake)
{
	m_pCameraShake = shake;
}

void ALightning::StrikeLightning(FVector location)
{
	m_bIsCounting = true;
	m_Location = location;

	if(m_pLightingStrikeSFX != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Red, FString::Printf(TEXT("lightning strike sound not found") ));
	}
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Red, FString::Printf(TEXT("lightning strike %f"), location.Z ));
	//DrawDebugSphere(GetWorld(), location, 50.0f, 4, FColor::Red, true, -1, 1, 4);
	
	UNiagaraComponent* niagaraSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
	 	GetWorld(),
	 	m_pNiagaraSystemStrike,
	 	m_Location,
	 	FRotator(1),
	 	FVector(1)
	 	);
	

	niagaraSystem->SetNiagaraVariableFloat("User.DurationPreStrike", m_LightningLifeTime);	
}

void ALightning::CheckStrike()
{
	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Red, FString::Printf(TEXT("lightning strike check") ));	
	
	TArray<FHitResult> hitResult;
	const FVector start = m_Location;
	const FVector end = m_Location;	

	GetWorld()->SweepMultiByChannel(
		 hitResult, 
		 start, 
		 end, 
		 FQuat::Identity, 
		 ECC_Pawn, // Your custom channel
		 FCollisionShape::MakeSphere(m_LightningRadius),
		 FCollisionQueryParams{}
		 );
			
	for (auto HitResult : hitResult)
	{		
		if( Cast<AMainCharacter>(HitResult.GetActor()))
		{
			Cast<AMainCharacter>(HitResult.GetActor())->DoDamageToSelf(m_LightningDamage);
		}
	}

	m_bIsCounting = false;
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pLightingStrikeSFX, m_Location);
	Destroy();
}



