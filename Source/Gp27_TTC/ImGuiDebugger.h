// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ImGuiDebugger.generated.h"

enum class ENumericType
{
	Int,
	Float
};

UCLASS()
class GP27_TTC_API UImGuiDebugger : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Initialize ImGui resources
	virtual void Init() override;

	// Clean up ImGui resources
	virtual void Shutdown() override;

	// Adds a checkbox linked to the provided boolean variable.
	void AddCheckbox(const FString& Label, bool* PtrToBool);

	void AddButton(const FString& Label, std::function<void()> Function);
	void AddNumericVariable(const FString& Label, void* PtrToVariable, ENumericType Type);


	// Render ImGui elements; should be called every frame.
	void RenderImGui();

private:
	// Structure to hold information about each checkbox
	struct FCheckboxInfo
	{
		FString Label;
		bool* PtrToBool;

		FCheckboxInfo(const FString& InLabel, bool* InPtrToBool)
			: Label(InLabel), PtrToBool(InPtrToBool) {}
	};

	struct FButtonInfo
	{
		FString Label;
		std::function<void()> Function;

		FButtonInfo(const FString& InLabel, std::function<void()> InFunction)
			: Label(InLabel), Function(InFunction) {}
	};

	// Structure to hold information about a numeric variable
	struct FNumericInfo
	{
		FString Label;
		void* PtrToVariable;
		ENumericType Type;

		FNumericInfo(const FString& InLabel, void* InPtrToVariable, ENumericType InType)
			: Label(InLabel), PtrToVariable(InPtrToVariable), Type(InType) {}
	};

	// List of all checkboxes to render
	TArray<FCheckboxInfo> Checkboxes;
	TArray<FButtonInfo> Buttons;
	TArray<FNumericInfo> Numerics;

};
