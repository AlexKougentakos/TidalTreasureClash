// Fill out your copyright notice in the Description page of Project Settings.


#include "ImGuiCallTest.h"

#include "ImGuiDebugger.h"

// Sets default values
AImGuiCallTest::AImGuiCallTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AImGuiCallTest::BeginPlay()
{
	Super::BeginPlay();
	UImGuiDebugger* MyGameInstance = Cast<UImGuiDebugger>(GetWorld()->GetGameInstance());
	if (MyGameInstance != nullptr)
	{
		MyGameInstance->AddCheckbox(TEXT("My Checkbox"), &m_bDoSomething);
		MyGameInstance->AddCheckbox(TEXT("My Checkbox 2"), &m_bDoSomethingElse);
		
		MyGameInstance->AddButton(TEXT("Do Something"), [this]() { DoSomething();});
	}

	
}

// Called every frame
void AImGuiCallTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UImGuiDebugger* MyGameInstance = Cast<UImGuiDebugger>(GetWorld()->GetGameInstance());
	if (MyGameInstance != nullptr)
	{
		MyGameInstance->RenderImGui();
	}
}


