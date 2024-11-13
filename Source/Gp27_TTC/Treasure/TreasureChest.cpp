#include "TreasureChest.h"

ATreasureChest::ATreasureChest()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ATreasureChest::BeginPlay()
{
	Super::BeginPlay();

	m_pChestMesh = FindComponentByClass<UStaticMeshComponent>();
	
	EnableSimulationMovementAndCollision(false);
	
	PutChestInTheGround();

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("DecomposeUpdate"));
	FOnTimelineEvent DecomposeFinished;
	DecomposeFinished.BindUFunction(this, FName("DecomposeFinished"));

	m_DecompositionTimeLine.AddInterpFloat(m_pDecompositionCurve, ProgressUpdate);
	m_DecompositionTimeLine.SetTimelineFinishedFunc(DecomposeFinished);

	m_pMaterial = m_pChestMesh->GetMaterial(0);
	m_pDynamicMaterial = UMaterialInstanceDynamic::Create(m_pMaterial, m_pChestMesh);
	m_pChestMesh->SetMaterial(0, m_pDynamicMaterial);
}


void ATreasureChest::Tick(float deltaTime)
{
	m_ElapsedAnimationTime += deltaTime;
	Super::Tick(deltaTime);

	//Spawn Animation
	

	if(m_bDecomposing)
	{
		m_DecompositionTimeLine.TickTimeline(deltaTime);
	}
	if (m_bCompletedAnimationSpawnAnimation) return;
	HandleSpawnAnimation(deltaTime);
}

void ATreasureChest::SetIsDecomposing(bool isDecomposing)
{
	m_bDecomposing = isDecomposing;
	m_DecompositionTimeLine.Play();
}

void ATreasureChest::HandleSpawnAnimation(const float deltaTime)
{
	if (m_ElapsedAnimationTime < m_SpawnAnimationTime)
	{
		const float rotation = m_RotationAmount * deltaTime / m_SpawnAnimationTime;
		AddActorLocalRotation(FRotator(0.0f, rotation, 0.0f));

		const float translation = m_TranslationAmount * deltaTime / m_SpawnAnimationTime;
		AddActorLocalOffset(FVector(0.0f, 0.0f, translation));
	}

	if (m_ElapsedAnimationTime >= m_SpawnAnimationTime && m_bCompletedAnimationSpawnAnimation == false)
	{
		m_bCompletedAnimationSpawnAnimation = true;
		m_ElapsedAnimationTime = 0.0f;
		EnableSimulationMovementAndCollision(true);
	}
}

void ATreasureChest::PutChestInTheGround()
{
	const UStaticMesh* staticMesh = m_pChestMesh->GetStaticMesh();
	const FBox boundingBox = staticMesh->GetBoundingBox();
	const float height = boundingBox.GetExtent().Z;
	const float boxDepth = height * 5.f;
	
	m_TranslationAmount = boxDepth + 10.0f;
	const FVector location = GetActorLocation();
	const FVector newLocation = FVector(location.X, location.Y, location.Z -  boxDepth);
	
	SetActorLocation(newLocation);
}

void ATreasureChest::DecomposeUpdate(float DeltaTime)
{
	//set material variable for decomposition
	m_Decomposition = m_pDecompositionCurve->GetFloatValue(m_DecompositionTimeLine.GetPlaybackPosition());
	m_pDynamicMaterial->SetScalarParameterValue(TEXT("Burn Amount"), m_Decomposition);
}

void ATreasureChest::DecomposeFinished()
{
	Destroy();
}

void ATreasureChest::EnableSimulationMovementAndCollision(const bool enable) const
{
	m_pChestMesh->SetSimulatePhysics(enable);
	
	m_pChestMesh->BodyInstance.bLockXTranslation = !enable;
	m_pChestMesh->BodyInstance.bLockYTranslation = !enable;
	m_pChestMesh->BodyInstance.bLockXRotation = !enable;
	m_pChestMesh->BodyInstance.bLockYRotation = !enable;

	if (!enable)
		m_pChestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	else 
		m_pChestMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}
