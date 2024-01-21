// Fill out your copyright notice in the Description page of Project Settings.


#include "RELobbyGameModeBase.h"

#include "REGameInstance.h"
#include "RELobbyPlayerController.h"

ARELobbyGameModeBase::ARELobbyGameModeBase()
{
	bUseSeamlessTravel = true;
}

void ARELobbyGameModeBase::StartPlay()
{
	Super::StartPlay();
	TimeLeftToStartMach = 120;
	GetWorldTimerManager().SetTimer(TimerHandle_StartSession, this, &ARELobbyGameModeBase::StartGame, 1.0f, true);
}

void ARELobbyGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (GetNumPlayers() >= 2 && TimeLeftToStartMach > 60)
		TimeLeftToStartMach = 60;
}

void ARELobbyGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (GetNumPlayers() <= 2)
	{
		TimeLeftToStartMach = 120;
	
	}
	// FString TET = "Player Left: " + FString::FromInt(GetNumPlayers());
	// if (GEngine != nullptr)
	// 	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TET);
}

void ARELobbyGameModeBase::StartGame()
{
	--TimeLeftToStartMach;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (const auto PC = Cast<ARELobbyPlayerController>(It->Get()))
			PC->ClientUpdateMatchStartUI(TimeLeftToStartMach);

	if(TimeLeftToStartMach > 0) return;

	GetWorldTimerManager().ClearTimer(TimerHandle_StartSession);

	if (UREGameInstance* GameInstance = Cast<UREGameInstance>(GetGameInstance()))
		GameInstance->StartSession();	

	if (UWorld* World = GetWorld())
		World->ServerTravel("/Game/_Main/Levels/MainMap?listen", true);
}
