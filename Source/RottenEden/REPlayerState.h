// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "REPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API AREPlayerState : public APlayerState
{
	GENERATED_BODY()

	AREPlayerState();

public:
	void AddScoreBonus(const int16& BonusMultiplier);
	
	void AddCreditsAndScore(const float& CreditsToAdd);

	int16 GetPlayerCredits() const { return PlayerCredits; }

private:
	UPROPERTY(Replicated)
	int16 PlayerCredits;
};
