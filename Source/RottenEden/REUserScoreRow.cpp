// Fill out your copyright notice in the Description page of Project Settings.


#include "REUserScoreRow.h"

void UREUserScoreRow::HighLightUserRow() const
{
	UserPlaceTxt->SetColorAndOpacity(FLinearColor::Green);
	UserNameTxt->SetColorAndOpacity(FLinearColor::Green);
	UserScoreTxt->SetColorAndOpacity(FLinearColor::Green);
}
