// Fill out your copyright notice in the Description page of Project Settings.


#include "REGameOverMenu.h"
#include "REUserScoreRow.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"
#include "REGameInstance.h"

bool UREGameOverMenu::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success || !RestartGameBtn || !QuitGameBtn) return false;

	RestartGameBtn->OnClicked.AddDynamic(this, &UREGameOverMenu::RestartGame);
	QuitGameBtn->OnClicked.AddDynamic(this, &UREGameOverMenu::GotoMainMenu);

	bIsFocusable = true;
	return true;
}

void UREGameOverMenu::SetUpGameOverMenu()
{
	APlayerState* OwningPS = GetOwningPlayerState();
	if (!OwningPS) return;

	ShowMatchUsersScore(OwningPS);
	RestartGameBtn->SetIsEnabled(GetWorld() ? GetWorld()->IsServer() : false);

	// Send server requests
	if (UREGameInstance* GameInstance = GetGameInstance<UREGameInstance>())
	{
		FUsersScore UserToUpDate;
		UserToUpDate.name = OwningPS->GetPlayerName();
		UserToUpDate.steamId = OwningPS->GetUniqueId().ToString();
		UserToUpDate.score = OwningPS->GetScore();

		FString ContentJsonString;
		FJsonObjectConverter::UStructToJsonObjectString(FUsersScore::StaticStruct(), &UserToUpDate, ContentJsonString,
		                                                0, 0);
		GameInstance->PostRequest(TEXT("/register"), ContentJsonString, [this, GameInstance, OwningPS](FString&& JsonContent)
		{
			GameInstance->GetRequest(TEXT("/scores"),
			                         [this, OwningPS](FString&& JsonContent)
			                         {
				                         ShowTopUsersScores(JsonContent, OwningPS);
			                         });
		});
	}
}

void UREGameOverMenu::ShowMatchUsersScore(APlayerState* OwningPS)
{
	AGameStateBase* MyGameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (!MyGameState) return;

	MyGameState->PlayerArray.Sort([](const APlayerState& a, const APlayerState& b)
	{
		return a.GetScore() > b.GetScore();
	});

	for (int i = 0; i < MyGameState->PlayerArray.Num(); ++i)
	{
		if (UREUserScoreRow* Row = CreateWidget<UREUserScoreRow>(this, UserScoreRowClass))
		{
			Row->SetUserPlace(
				FText::FromString(FString::FromInt(i + 1) + ". "));
			Row->SetUserName(FText::FromString(MyGameState->PlayerArray[i]->GetPlayerName() + " "));
			Row->SetUserScore(FText::FromString(FString::SanitizeFloat(MyGameState->PlayerArray[i]->GetScore())));

			if (OwningPS->GetPlayerName() == MyGameState->PlayerArray[i]->GetPlayerName())
				Row->HighLightUserRow();

			PlayersMatchScore->AddChild(Row);
		}
	}
}

void UREGameOverMenu::ShowTopUsersScores(const FString& JsonContent, APlayerState* OwningPS)
{
	TArray<FUsersScore> UsersScores;
	FJsonObjectConverter::JsonArrayStringToUStruct(JsonContent, &UsersScores, 0, 0);

	for (int i = 0; i < UsersScores.Num(); ++i)
	{
		if (UREUserScoreRow* Row = CreateWidget<UREUserScoreRow>(this, UserScoreRowClass))
		{
			Row->SetUserPlace(
				FText::FromString(FString::FromInt(i + 1) + ". "));
			Row->SetUserName(FText::FromString(UsersScores[i].name + " "));
			Row->SetUserScore(FText::FromString(FString::SanitizeFloat(UsersScores[i].score)));
			
			if (OwningPS->GetPlayerName() == UsersScores[i].name &&
				OwningPS->GetUniqueId().ToString() == UsersScores[i].steamId)
				Row->HighLightUserRow();

			LeaderBoard->AddChild(Row);
		}
	}
}

void UREGameOverMenu::RestartGame()
{
	if (UWorld* World = GetWorld())
	{
		if (World->IsServer())
		{
			World->ServerTravel("/Game/_Main/Levels/MainMap?listen", true);
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonConfirmationSound);
		}
		else
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonFailureSound);
	}
}

void UREGameOverMenu::GotoMainMenu()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SpawnSound2D(GetWorld(), ButtonConfirmationSound);
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
