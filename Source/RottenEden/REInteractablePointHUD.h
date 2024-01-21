// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "REInteractablePointHUD.generated.h"

class UTextBlock;
class UButton;
class UInGameHUD;
class UImage;
class AREDefenceBuildingPlace;

// should be an interface or inherited class
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnVisibleSignatures, UUserWidget*, Broadcaster, const bool, IsVisible);

UCLASS()
class ROTTENEDEN_API UREInteractablePointHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class AREInteractablePlaceBase* InteractablePlace;

	FOnVisibleSignatures OnVisible;

	void SwitchMenu();

	void SetInteractablePlaceBase(class AREInteractablePlaceBase* Place);

	void SetPrompText(FText&& Message) const;

	void SetMenuIndex(const int8& Index) { CurrentMenuIndex = Index; }

	void SetPlayerHud(UInGameHUD* PlayerHUDReference) { PlayerHUD = PlayerHUDReference; }

	void SetWarningText(FString text);

	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	virtual void RemoveFromParent() override;

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TArray<TSubclassOf<class AREGun>> AvailableGuns;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UREWeaponShopRow> WeaponRowClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* ButtonConfirmationSound;

	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* ButtonFailureSound;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PromptText;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitStoreBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* ExitStoreBtn_1;

	UPROPERTY(meta = (BindWidget))
	UButton* BuyMedkitsBtn;

	UPROPERTY(meta = (BindWidget))
	UWidget* PromptMenu;

	UPROPERTY(meta = (BindWidget))
	UWidget* DefenceMenu;

	UPROPERTY(meta = (BindWidget))
	UButton* MoveMenuLeft;

	UPROPERTY(meta = (BindWidget))
	UButton* MoveMenuRight;

	UPROPERTY(meta = (BindWidget))
	UButton* Buybutton;

	UPROPERTY(meta = (BindWidget))
	UButton* Repairbutton;

	UPROPERTY(meta = (BindWidget))
	UButton* Sellbutton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Defencename;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefenceStats;

	UPROPERTY(meta = (BindWidget))
	UImage* DefenceImage;

	UPROPERTY(meta = (BindWidget))
	UWidget* ShopMenu;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalCreditsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalCreditsText1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeltaCreditsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ActionText;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* CreditsTextAnim;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MedkitPriceText;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* ShopWeaponsContainer;

	UPROPERTY()
	UInGameHUD* PlayerHUD;

	int8 CurrentMenuIndex;
	int8 CurrentDefenceIndex;

	UFUNCTION()
	void BuyMedkit();

	UFUNCTION()
	void ExitShopMenu();

	UFUNCTION()
	void UpdateShopCreditsUI(const int16& ItemPrice);

	void UpdateShopCreditsUI(UTextBlock* StoreCreditsUI, const float& ItemPrice);


	void ChangeCurrentDefenseIndex(bool Increase);

	UFUNCTION()
	void LoadDefence(class AREDefence* CurrentDefence);

	UFUNCTION()
	void MoveLeftMenu();

	UFUNCTION()
	void MoveRightMenu();

	UFUNCTION()
	void BuyReplaceDefence();

	UFUNCTION()
	void RepairDefence();

	UFUNCTION()
	void SellDefence();
	
	UFUNCTION()
	void UpdateDefenceButton(AREDefenceBuildingPlace* DefencePlace);

	class UREInventoryComponent* GetOwnerPlayerInventory() const;
};
