// Fill out your copyright notice in the Description page of Project Settings.


#include "REServerRow.h"


#include "REMainMenu.h"
#include "Components/Button.h"

UREServerRow::UREServerRow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsHeader = false;
	bSelected = false;
}

bool UREServerRow::Initialize()
{
	const bool Success = Super::Initialize();
	
	if (!Success || !ServerNameButton) return false;
	ServerNameButton->OnClicked.AddDynamic(this, &UREServerRow::SetChosenServer);
	ServerNameButton->OnHovered.AddDynamic(this, &UREServerRow::OnHovered);
	ServerNameButton->OnUnhovered.AddDynamic(this, &UREServerRow::OnUnhovered);
	return true;
}

void UREServerRow::SetUp(UREMainMenu* Parent, uint32 Index)
{
	MainMenu = Parent;
	ButtonIndex = Index;
}

void UREServerRow::ResetColor()
{
	if (bIsHeader) return;
	ServerNameButton->SetColorAndOpacity(FLinearColor::White);
}

void UREServerRow::SetChosenServer()
{
	if (!MainMenu) return;
	MainMenu->SetSelectedIndex(ButtonIndex);
	ServerNameButton->SetColorAndOpacity(FLinearColor::Green);
}

void UREServerRow::OnHovered()
{
	if (bIsHeader) return;
	if (!bSelected)
		ServerNameButton->SetColorAndOpacity(FLinearColor::Yellow);
}

void UREServerRow::OnUnhovered()
{
	if (bIsHeader) return;
	if (!bSelected)
		ServerNameButton->SetColorAndOpacity(FLinearColor::White);
}
