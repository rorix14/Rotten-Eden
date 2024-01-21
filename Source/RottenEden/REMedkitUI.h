// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "REMedkitUI.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UREMedkitUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UWidgetAnimation* GetUseMedkitAnim() const { return UseMedkitAnim; }

	bool HasUsed = false;

private:
	UPROPERTY(meta = (BindWidget))
	UImage* MedkitImage;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* UseMedkitAnim;
};
