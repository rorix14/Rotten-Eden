// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REPlayerController.h"
#include "RELobbyPlayerController.generated.h"

/**
 * 
 */
class URETutotrialUI;

UCLASS()
class ROTTENEDEN_API ARELobbyPlayerController : public AREPlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(Client, Reliable)
	void ClientUpdateMatchStartUI(const int16& TimeRemaining);

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<URETutotrialUI> LobbyUIClass;

	UPROPERTY()
	URETutotrialUI* LobbyUI;
};
