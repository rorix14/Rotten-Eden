// Fill out your copyright notice in the Description page of Project Settings.


#include "RETutotrialUI.h"

void URETutotrialUI::SetTimeReamingText(const int16& TimeRemaining) const
{
	TimeForGameStartText->SetText(FText::FromString("Game starts in: " + FString::FromInt(static_cast<int>(TimeRemaining))));
}
