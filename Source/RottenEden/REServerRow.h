// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "REServerRow.generated.h"

/**
 * 
 */
class UButton;
class UTextBlock;
class UREMainMenu;

UCLASS()
class ROTTENEDEN_API UREServerRow : public UUserWidget
{
	GENERATED_BODY()

	UREServerRow(const FObjectInitializer& ObjectInitializer);

public:
	void SetUp(UREMainMenu* Parent, uint32 Index);

	void ResetColor();

	UPROPERTY(EditInstanceOnly)
	bool bIsHeader;

	UPROPERTY()
	bool bSelected;

	class UTextBlock* GetServerSessionText() const { return ServerName; }

	class UTextBlock* GetHostUser() const { return HostUser; }

	class UTextBlock* GetConnectionFraction() const { return ConnectionFraction; }

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	UButton* ServerNameButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ServerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HostUser;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ConnectionFraction;

	int32 ButtonIndex;

	// use a delegate instead 
	UPROPERTY()
	UREMainMenu* MainMenu;

	UFUNCTION()
    void SetChosenServer();

	UFUNCTION()
    void OnHovered();

	UFUNCTION()
    void OnUnhovered();
};
