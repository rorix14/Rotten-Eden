// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RELobbyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API ARELobbyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ARELobbyGameModeBase();

public:	
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void StartPlay() override;

private:
	void StartGame();

	// Default match start time in seconds
	UPROPERTY(EditDefaultsOnly)
	int16 TimeLeftToStartMach;

	FTimerHandle TimerHandle_StartSession;
};
