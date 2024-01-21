// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "RETutotrialUI.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API URETutotrialUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTimeReamingText(const int16& TimeRemaining) const;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeForGameStartText;
};
