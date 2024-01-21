// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "REGameOverMenu.generated.h"

/**
 * 
 */
USTRUCT()
struct FUsersScore
{
	GENERATED_BODY()

	UPROPERTY()
	FString name;

	UPROPERTY()
	FString steamId;

	UPROPERTY()
	float score;
};

class UVerticalBox;
class UButton;
UCLASS()
class ROTTENEDEN_API UREGameOverMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetUpGameOverMenu();
	
protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UREUserScoreRow> UserScoreRowClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* ButtonConfirmationSound;

	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* ButtonFailureSound;
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayersMatchScore;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* LeaderBoard;

	UPROPERTY(meta = (BindWidget))
	UButton* RestartGameBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitGameBtn;

	void ShowMatchUsersScore(APlayerState* OwningPS);

	void ShowTopUsersScores(const FString& JsonContent, APlayerState* OwningPS);
	
	UFUNCTION()
	void RestartGame();
	
	UFUNCTION()
	void GotoMainMenu();
};
