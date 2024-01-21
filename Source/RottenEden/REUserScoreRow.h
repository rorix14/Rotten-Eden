// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "REUserScoreRow.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class ROTTENEDEN_API UREUserScoreRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetUserPlace(FText&& UserPlace) const { UserPlaceTxt->SetText(UserPlace); }

	void SetUserName(FText&& UserName) const { UserNameTxt->SetText(UserName); }

	void SetUserScore(FText&& UserScore) const { UserScoreTxt->SetText(UserScore); }

	void HighLightUserRow() const;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* UserPlaceTxt;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UserNameTxt;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UserScoreTxt;
};
