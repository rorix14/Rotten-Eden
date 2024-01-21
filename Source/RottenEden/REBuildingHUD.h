// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "REBuildingHUD.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UREBuildingHUD : public UUserWidget
{
	GENERATED_BODY()

	public:

	virtual void turnOnOffHUD(bool state);

	virtual void changeDefenseName(FString name);

	void changeText(FString text);

	protected:

	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

	private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* defenseName;

	/*
	UPROPERTY(meta = (BindWidget))
		UButton* leftButton;

	UPROPERTY(meta = (BindWidget))
		UButton* rightButton;
		*/

	bool menuHidden;
	
};
