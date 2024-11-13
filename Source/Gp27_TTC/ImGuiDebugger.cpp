// Fill out your copyright notice in the Description page of Project Settings.


#include "ImGuiDebugger.h"
#include "imgui.h"

void UImGuiDebugger::Init()
{
	Super::Init();
}

void UImGuiDebugger::Shutdown()
{
	// Cleanup ImGui here (if necessary)
	Super::Shutdown();
}

void UImGuiDebugger::AddCheckbox(const FString& Label, bool* PtrToBool)
{
	Checkboxes.Add(FCheckboxInfo(Label, PtrToBool));
}

void UImGuiDebugger::AddButton(const FString& Label, std::function<void()> Function)
{
	Buttons.Add(FButtonInfo(Label, Function));
}

void UImGuiDebugger::AddNumericVariable(const FString& Label, void* PtrToVariable, ENumericType Type)
{
	Numerics.Add(FNumericInfo(Label, PtrToVariable, Type));
}


void UImGuiDebugger::RenderImGui()
{
#if WITH_EDITOR
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	ImGui::Begin("ImGui Debugger");
	
	for (const FCheckboxInfo& Checkbox : Checkboxes)
	{
		if (Checkbox.PtrToBool)
		{
			ImGui::Checkbox(TCHAR_TO_UTF8(*Checkbox.Label), Checkbox.PtrToBool);
		}
	}

	// Render buttons
	for (const FButtonInfo& Button : Buttons)
	{
		if (ImGui::Button(TCHAR_TO_UTF8(*Button.Label)))
		{
			Button.Function(); // Call the linked function
		}
	}

	for (const FNumericInfo& Numeric : Numerics)
	{
		switch (Numeric.Type)
		{
		case ENumericType::Float:
			ImGui::DragFloat(TCHAR_TO_UTF8(*Numeric.Label), static_cast<float*>(Numeric.PtrToVariable));
			break;
		case ENumericType::Int:
			ImGui::InputInt(TCHAR_TO_UTF8(*Numeric.Label), static_cast<int*>(Numeric.PtrToVariable));
			break;
		}
	}
	
	// Render other ImGui widgets as needed

	ImGui::End();
#endif
}
