// Fill out your copyright notice in the Description page of Project Settings.


#include "CannonBall.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Gp27_TTC/Character/MainCharacter.h"
#include "Gp27_TTC/Water/WaterLevel.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACannonBall::ACannonBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_pProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("projectile movement Component"));

	m_SplashFactor = 2.5f;
	m_bDeleteWhenInWater = false;
}

// Called when the game starts or when spawned
void ACannonBall::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACannonBall::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other->IsA(AWaterLevel::StaticClass())) return;

	TArray<FHitResult> hitResult;
	
	GetWorld()->SweepMultiByChannel(
		 hitResult, 
		 Hit.Location, 
		 Hit.Location, 
		 FQuat::Identity, 
		 ECC_Pawn, // Your custom channel
		 FCollisionShape::MakeSphere(m_ExplosionRadius),
		 FCollisionQueryParams{}
		 );
			
	for (auto HitResult : hitResult)
	{		
		if( Cast<AMainCharacter>(HitResult.GetActor()))
		{
			Cast<AMainCharacter>(HitResult.GetActor())->DoDamageToSelf(m_ExplosionDamage);
		}
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		 GetWorld(),
		 m_pExplosionNiagaraSystem,
		 HitLocation,
		 FRotator{90,0,0},
		 FVector(2.f)
		 );
   
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), m_pCannonBallExplosionSound, GetActorLocation());
	//Apply camera shake
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientStartCameraShake(m_pCameraShake);
	
	Destroy();
}

// Called every frame
void ACannonBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



