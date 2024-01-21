// Fill out your copyright notice in the Description page of Project Settings.


#include "REMainMenu.h"
#include "REServerRow.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/KismetSystemLibrary.h"

bool UREMainMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success || !HostButton || !GotoJoinMenuButton || !BackButton1 || !BackButton2 || !JoinButton || !
		GotoHostMenuButton || !LeaveGame)
		return false;

	HostButton->OnClicked.AddDynamic(this, &UREMainMenu::HostServer);
	GotoJoinMenuButton->OnClicked.AddDynamic(this, &UREMainMenu::OpenJoinMenu);
	BackButton1->OnClicked.AddDynamic(this, &UREMainMenu::OpenMainMenu);
	BackButton2->OnClicked.AddDynamic(this, &UREMainMenu::OpenMainMenu);
	JoinButton->OnClicked.AddDynamic(this, &UREMainMenu::JoinServer);
	GotoHostMenuButton->OnClicked.AddDynamic(this, &UREMainMenu::OpenHostMenu);
	LeaveGame->OnClicked.AddDynamic(this, &UREMainMenu::QuitGame);

	bIsFocusable = true;

	return true;
}

void UREMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		FInputModeUIOnly InputModeUI;
		InputModeUI.SetWidgetToFocus(this->TakeWidget());
		InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputModeUI);
		PlayerController->bShowMouseCursor = true;
	}

	if ((GameInstance = GetGameInstance<UREGameInstance>()) != nullptr)
		GameInstance->OnGetSessionsList.AddDynamic(this, &UREMainMenu::SetServerList);
}

void UREMainMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		const FInputModeGameOnly InputModeGame;
		PlayerController->SetInputMode(InputModeGame);
		PlayerController->bShowMouseCursor = false;
	}

	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UREMainMenu::SetServerList(const TArray<FServerData>& ServerNames)
{
	ServerList->ClearChildren();

	uint32 Index = 0;
	for (const FServerData& ServerData : ServerNames)
	{
		if (UREServerRow* Row = CreateWidget<UREServerRow>(this, ServerRow))
		{
			Row->GetServerSessionText()->SetText(FText::FromString(ServerData.Name));
			Row->GetHostUser()->SetText(FText::FromString(ServerData.HostName));
			Row->GetConnectionFraction()->SetText(
				FText::FromString(FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers)));
			Row->SetUp(this, Index);
			ServerList->AddChild(Row);
		}
	
		++Index;
	}
}

void UREMainMenu::SetSelectedIndex(TOptional<int32> Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UREMainMenu::HostServer()
{
	if (GameInstance)
		GameInstance->Host(ServerHostName->GetText().ToString());
}

void UREMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && GameInstance)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Slected index %d"), SelectedIndex.GetValue());
		GameInstance->Join(SelectedIndex.GetValue());
	}
}

void UREMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); i++)
	{
		if (UREServerRow* Row = Cast<UREServerRow>(ServerList->GetChildAt(i)))
		{
			Row->bSelected = SelectedIndex.IsSet() && SelectedIndex.GetValue() == i;
			Row->ResetColor();
		}
	}
}

void UREMainMenu::OpenJoinMenu()
{
	if (!GameInstance) return;

	MenuSwitcher->SetActiveWidget(JoinMenu);
	GameInstance->RefreshServerList();
}

void UREMainMenu::OpenMainMenu()
{
	MenuSwitcher->SetActiveWidget(MainMenu);
	SelectedIndex.Reset();
}

void UREMainMenu::OpenHostMenu()
{
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UREMainMenu::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}
