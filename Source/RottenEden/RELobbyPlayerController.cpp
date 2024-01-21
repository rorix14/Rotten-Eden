// Fill out your copyright notice in the Description page of Project Settings.


#include "RELobbyPlayerController.h"
#include "RETutotrialUI.h"
#include "Blueprint/UserWidget.h"

void ARELobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!LobbyUI)
	{
		if((LobbyUI = CreateWidget<URETutotrialUI>(this, LobbyUIClass)) != nullptr)
			LobbyUI->AddToViewport();
	}
}

void ARELobbyPlayerController::ClientUpdateMatchStartUI_Implementation(const int16& TimeRemaining)
{
	if(LobbyUI)
		LobbyUI->SetTimeReamingText(TimeRemaining);
}

void ARELobbyPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType,
	bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	if(LobbyUI)
		LobbyUI->RemoveFromViewport();
}

void ARELobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
