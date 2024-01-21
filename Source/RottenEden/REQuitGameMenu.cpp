// Fill out your copyright notice in the Description page of Project Settings.


#include "REQuitGameMenu.h"

#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"

bool UREQuitGameMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success || !GotoMainMenuButton || !CancelButton) return false;

	GotoMainMenuButton->OnClicked.AddDynamic(this, &UREQuitGameMenu::GotoMainMenu);
	CancelButton->OnClicked.AddDynamic(this, &UREQuitGameMenu::InteractWithQuitGameMenu);

	bIsFocusable = true;
	return true;
}

// Can use set visibility instead, although it might have unforeseen side effects 
void UREQuitGameMenu::InteractWithQuitGameMenu()
{
	if (GetVisibility() == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::Visible);
		OnVisible.Broadcast(this, true);
		return;
	}

	if (GetVisibility() == ESlateVisibility::Visible)
	{
		SetVisibility(ESlateVisibility::Hidden);
		OnVisible.Broadcast(this, false);
	}
}

void UREQuitGameMenu::RemoveFromParent()
{
	if(GetVisibility() == ESlateVisibility::Visible)
		OnVisible.Broadcast(this, false);

	OnVisible.Clear();
	Super::RemoveFromParent();
}

void UREQuitGameMenu::GotoMainMenu()
{
	if (UWorld* World = GetWorld())
	{
		if (World->IsServer())
		{
			if (AGameModeBase* GameMode = World->GetAuthGameMode())
				GameMode->ReturnToMainMenuHost();
		}
		else
		{
			if (APlayerController* PC = GetOwningPlayer())
				PC->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("Back to main menu")));
		}
	}
}
