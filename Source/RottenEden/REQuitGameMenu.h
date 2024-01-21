// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "REQuitGameMenu.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVisibleSignature, UUserWidget*, Broadcaster, const bool, IsVisible);

UCLASS()
class ROTTENEDEN_API UREQuitGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnVisibleSignature OnVisible;

	UFUNCTION()
	void InteractWithQuitGameMenu();
	
	virtual void RemoveFromParent() override;

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* GotoMainMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UFUNCTION()
	void GotoMainMenu();
};
