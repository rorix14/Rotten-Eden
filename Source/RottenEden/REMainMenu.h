// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "REGameInstance.h"
#include "REMainMenu.generated.h"

class UButton;
class UWidget;
class UWidgetSwitcher;
class UPanelWidget;
class UEditableTextBox;
/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UREMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	UFUNCTION()
	void SetServerList(const TArray<FServerData>& ServerNames);

	void SetSelectedIndex(TOptional<int32> Index);

protected:
	virtual bool Initialize() override;

	virtual void NativeConstruct() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UREServerRow> ServerRow;
	
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* GotoJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* GotoHostMenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton1;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton2;

	UPROPERTY(meta = (BindWidget))
	UButton* LeaveGame;

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	UPanelWidget* ServerList;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ServerHostName;

	UPROPERTY()
	class UREGameInstance* GameInstance;

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void QuitGame();

	TOptional<int32> SelectedIndex;

	void UpdateChildren();
};
