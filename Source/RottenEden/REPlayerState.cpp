// Fill out your copyright notice in the Description page of Project Settings.


#include "REPlayerState.h"
#include <Net/UnrealNetwork.h>

AREPlayerState::AREPlayerState()
{
	PlayerCredits = 0;
}

void AREPlayerState::AddScoreBonus(const int16& BonusMultiplier)
{
	// can use a more complex scoring system to make scoring more interesting
	SetScore(GetScore() * BonusMultiplier);
}

void AREPlayerState::AddCreditsAndScore(const float& CreditsToAdd)
{
	PlayerCredits += CreditsToAdd;
	FMath::Clamp(PlayerCredits,static_cast<int16>(0), PlayerCredits);
	if (CreditsToAdd > 0)
		SetScore(GetScore() + CreditsToAdd);
}

void AREPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AREPlayerState, PlayerCredits, COND_OwnerOnly);
}
