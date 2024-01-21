// Fill out your copyright notice in the Description page of Project Settings.


#include "REBuildingHUD.h"

void UREBuildingHUD::turnOnOffHUD(bool state)
{
	if (state)
	{
		defenseName->SetVisibility(ESlateVisibility::Visible);
	} 
	else
	{
		defenseName->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UREBuildingHUD::changeDefenseName(FString name)
{
	FText text = FText::FromString("Build " + name + "\nPress 'Enter' to build press 'E' to exit.");
	defenseName->SetText(text);
}

void UREBuildingHUD::changeText(FString phrase)
{
	FText text = FText::FromString(phrase);
	defenseName->SetText(text);
}

bool UREBuildingHUD::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success) return false;
	menuHidden = true;
	defenseName->SetVisibility(ESlateVisibility::Hidden);

	return true;
}

void UREBuildingHUD::NativeConstruct()
{
	Super::NativeConstruct();
}


