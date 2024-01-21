// Fill out your copyright notice in the Description page of Project Settings.


#include "REInteractablePointHUD.h"
#include "InGameHUD.h"
#include "REInventoryComponent.h"
#include "REFPGun.h"
#include "REWeaponShopRow.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/GameplayStatics.h"
#include "REDefenceBuildingPlace.h"
#include "REDefence.h"
#include "Components/Image.h"

bool UREInteractablePointHUD::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success || !BuyMedkitsBtn || !ExitStoreBtn || !ExitStoreBtn_1 || !MoveMenuLeft || !MoveMenuRight || !Buybutton
		|| !Repairbutton || !Sellbutton)
		return false;

	BuyMedkitsBtn->OnClicked.AddDynamic(this, &UREInteractablePointHUD::BuyMedkit);
	ExitStoreBtn->OnClicked.AddDynamic(this, &UREInteractablePointHUD::ExitShopMenu);
	ExitStoreBtn_1->OnClicked.AddDynamic(this, &UREInteractablePointHUD::ExitShopMenu);
	MoveMenuLeft->OnClicked.AddDynamic(this, &UREInteractablePointHUD::MoveLeftMenu);
	MoveMenuRight->OnClicked.AddDynamic(this, &UREInteractablePointHUD::MoveRightMenu);
	Buybutton->OnClicked.AddDynamic(this, &UREInteractablePointHUD::BuyReplaceDefence);
	Repairbutton->OnClicked.AddDynamic(this, &UREInteractablePointHUD::RepairDefence);
	Sellbutton->OnClicked.AddDynamic(this, &UREInteractablePointHUD::SellDefence);

	for (int i = 0; i < AvailableGuns.Num(); ++i)
	{
		UREWeaponShopRow* WeaponRow = CreateWidget<UREWeaponShopRow>(this, WeaponRowClass);
		WeaponRow->SetUpWeaponRow(AvailableGuns[i]);
		WeaponRow->OnByItemEvent.AddDynamic(this, &UREInteractablePointHUD::UpdateShopCreditsUI);
		ShopWeaponsContainer->AddChildToUniformGrid(WeaponRow, i / 2, i % 2);
	}

	if (const auto PlayerInventory = GetOwnerPlayerInventory())
		MedkitPriceText->SetText(FText::FromString(FString::FromInt(PlayerInventory->GetMedkitPrice())));

	//CurrentMenuIndex = 0;
	CurrentDefenceIndex = 0;
	bIsFocusable = true;
	return true;
}

void UREInteractablePointHUD::SetPrompText(FText&& Message) const
{
	PromptText->SetText(Message);
}

void UREInteractablePointHUD::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (InVisibility == ESlateVisibility::Hidden)
		SwitchMenu();
}

void UREInteractablePointHUD::RemoveFromParent()
{
	if (MenuSwitcher && MenuSwitcher->GetWidgetAtIndex(CurrentMenuIndex) != PromptMenu)
		ExitShopMenu();

	OnVisible.Clear();
	Super::RemoveFromParent();
}

void UREInteractablePointHUD::SwitchMenu()
{
	if (MenuSwitcher && MenuSwitcher->GetActiveWidgetIndex() == CurrentMenuIndex) return;

	MenuSwitcher->SetActiveWidgetIndex(CurrentMenuIndex);

	if (MenuSwitcher->GetWidgetAtIndex(CurrentMenuIndex) == ShopMenu)
	{
		// should get credits from player state credits instead of Player HUD
		if (PlayerHUD)
			TotalCreditsText->SetText(PlayerHUD->GetTotalCreditsText()->GetText());
		// Double assignment might problems  
		OnVisible.Broadcast(this, true);
	}

	if (MenuSwitcher->GetWidgetAtIndex(CurrentMenuIndex) == DefenceMenu)
	{
		if (PlayerHUD)
			TotalCreditsText1->SetText(PlayerHUD->GetTotalCreditsText()->GetText());

		if (auto DefenceBuildingPlace = Cast<AREDefenceBuildingPlace>(InteractablePlace))
		{
			CurrentDefenceIndex = 0;
			UpdateDefenceButton(DefenceBuildingPlace);
			LoadDefence(DefenceBuildingPlace->ReturnCurrentDefence(CurrentDefenceIndex));
		}

		OnVisible.Broadcast(this, true);
	}
}

void UREInteractablePointHUD::SetInteractablePlaceBase(class AREInteractablePlaceBase* Place)
{
	InteractablePlace = Place;
}

void UREInteractablePointHUD::BuyMedkit()
{
	if (const auto PlayerInventory = GetOwnerPlayerInventory())
	{
		if (PlayerInventory->CanBuyHealthPack())
		{
			UpdateShopCreditsUI(PlayerInventory->GetMedkitPrice());
			PlayerInventory->ServerBuyHealthPack();
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonConfirmationSound);
		}
		else
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonFailureSound);
	}
}

void UREInteractablePointHUD::UpdateShopCreditsUI(const int16& ItemPrice)
{
	// Update shop dummy player credits UI text locally
	DeltaCreditsText->SetText(FText::FromString(FString::FromInt(-ItemPrice)));
	TotalCreditsText->SetText(FText::FromString(
		FString::FromInt(FCString::Atoi(*TotalCreditsText->GetText().ToString()) - ItemPrice)));
	PlayAnimation(CreditsTextAnim);
}

void UREInteractablePointHUD::UpdateShopCreditsUI(UTextBlock* StoreCreditsUI, const float& ItemPrice)
{
	StoreCreditsUI->SetText(FText::FromString(
		FString::FromInt(FCString::Atoi(*StoreCreditsUI->GetText().ToString()) - ItemPrice)));
}

void UREInteractablePointHUD::MoveLeftMenu()
{
	if (auto DefenceBuildingPlace = Cast<AREDefenceBuildingPlace>(InteractablePlace))
	{
		ChangeCurrentDefenseIndex(true);
		UpdateDefenceButton(DefenceBuildingPlace);
		LoadDefence(DefenceBuildingPlace->ReturnCurrentDefence(CurrentDefenceIndex));
	}
}

void UREInteractablePointHUD::MoveRightMenu()
{
	if (auto DefenceBuildingPlace = Cast<AREDefenceBuildingPlace>(InteractablePlace))
	{
		ChangeCurrentDefenseIndex(false);
		UpdateDefenceButton(DefenceBuildingPlace);
		LoadDefence(DefenceBuildingPlace->ReturnCurrentDefence(CurrentDefenceIndex));
	}
}

void UREInteractablePointHUD::BuyReplaceDefence()
{
	if (auto DefenceBuildingPlace = Cast<AREDefenceBuildingPlace>(InteractablePlace))
	{
		if (const auto PlayerInventory = GetOwnerPlayerInventory())
		{
			FString Reason;
			if (PlayerInventory->CanBuyDefence(DefenceBuildingPlace,
			                                   DefenceBuildingPlace->GetDefenceClass(CurrentDefenceIndex), Reason))
			{
				PlayerInventory->ServerBuyDefence(DefenceBuildingPlace,
				                                  DefenceBuildingPlace->GetDefenceClass(CurrentDefenceIndex));
				UpdateShopCreditsUI(TotalCreditsText1,
				                    DefenceBuildingPlace->
				                    GetDefenceClass(CurrentDefenceIndex)->GetDefaultObject<AREDefence>()->
				                    DefencePrice);
				Buybutton->SetIsEnabled(false);
				Repairbutton->SetIsEnabled(true);
				Sellbutton->SetIsEnabled(true);
				SetWarningText(Reason);
				UGameplayStatics::SpawnSound2D(GetWorld(), ButtonConfirmationSound);
			}
			else
			{
				SetWarningText(Reason);
				UGameplayStatics::SpawnSound2D(GetWorld(), ButtonFailureSound);
			}
		}
	}
}

void UREInteractablePointHUD::RepairDefence()
{
	if (auto DefenceBuildingPlace = Cast<AREDefenceBuildingPlace>(InteractablePlace))
	{
		if (DefenceBuildingPlace->IsDefenceBuilt())
		{
			if (const auto PlayerInventory = GetOwnerPlayerInventory())
			{
				FString Reason;
				if (PlayerInventory->CanBuyRepairDefence(DefenceBuildingPlace,
				                                         DefenceBuildingPlace->GetDefenceClass(CurrentDefenceIndex),
				                                         Reason))
				{
					PlayerInventory->ServerBuyRepairDefence(DefenceBuildingPlace,
					                                        DefenceBuildingPlace->GetDefenceClass(CurrentDefenceIndex));
					UpdateShopCreditsUI(TotalCreditsText1,
					                    DefenceBuildingPlace->
					                    GetDefenceClass(CurrentDefenceIndex)->GetDefaultObject<AREDefence>()->
					                    DefencePrice * 0.20);

					UGameplayStatics::SpawnSound2D(GetWorld(), ButtonConfirmationSound);
				}
				else
					UGameplayStatics::SpawnSound2D(GetWorld(), ButtonFailureSound);

				SetWarningText(Reason);
			}
		}
		else
		{
			UpdateDefenceButton(DefenceBuildingPlace);
		}
	}
}

void UREInteractablePointHUD::SellDefence()
{
	if (auto DefenceBuildingPlace = Cast<AREDefenceBuildingPlace>(InteractablePlace))
	{
		if (DefenceBuildingPlace->IsDefenceBuilt())
		{
			if (const auto PlayerInventory = GetOwnerPlayerInventory())
			{
				UpdateShopCreditsUI(TotalCreditsText1,
                            -DefenceBuildingPlace->
                             GetDefenceClass(CurrentDefenceIndex)->GetDefaultObject<AREDefence>()->
                             DefencePrice * DefenceBuildingPlace->GetDefenceHealthPercentage());
				
				PlayerInventory->ServerReturnMoneySellDefence(DefenceBuildingPlace,
				                                              DefenceBuildingPlace->
				                                              GetDefenceClass(CurrentDefenceIndex));
				
				Buybutton->SetIsEnabled(true);
				Repairbutton->SetIsEnabled(false);
				Sellbutton->SetIsEnabled(false);
				SetWarningText("Defence sold");
				UGameplayStatics::SpawnSound2D(GetWorld(), ButtonConfirmationSound);
			}
		}
		else
		{
			UpdateDefenceButton(DefenceBuildingPlace);
		}
	}
}

void UREInteractablePointHUD::UpdateDefenceButton(AREDefenceBuildingPlace* DefencePlace)
{
	if (DefencePlace->IsCurrentDefenceEqualToMenuIndex(CurrentDefenceIndex))
	{
		Buybutton->SetIsEnabled(false);
		Repairbutton->SetIsEnabled(true);
		Sellbutton->SetIsEnabled(true);
	}
	else if (!DefencePlace->IsDefenceBuilt())
	{
		Buybutton->SetIsEnabled(true);
		Repairbutton->SetIsEnabled(false);
		Sellbutton->SetIsEnabled(false);
	}
	else
	{
		Buybutton->SetIsEnabled(false);
		Repairbutton->SetIsEnabled(false);
		Sellbutton->SetIsEnabled(false);
	}
}

void UREInteractablePointHUD::ChangeCurrentDefenseIndex(bool Increase)
{
	int32 DefenceArraySize = Cast<AREDefenceBuildingPlace>(InteractablePlace)->ReturnBuildableDefenceArraySize();
	if (Increase)
	{
		CurrentDefenceIndex++;
		if (CurrentDefenceIndex >= DefenceArraySize) CurrentDefenceIndex = 0;
	}
	else
	{
		CurrentDefenceIndex--;
		if (CurrentDefenceIndex < 0) CurrentDefenceIndex = DefenceArraySize - 1;
	}
}

void UREInteractablePointHUD::LoadDefence(AREDefence* CurrentDefence)
{
	Defencename->SetText(FText::FromString(CurrentDefence->ReturnName()));
	DefenceStats->SetText(FText::FromString(CurrentDefence->ReturnStats()));
	DefenceImage->SetBrushFromTexture(CurrentDefence->DefenceUIImage);
	SetWarningText("");
}

void UREInteractablePointHUD::SetWarningText(FString text)
{
	ActionText->SetText(FText::FromString(text));
}

void UREInteractablePointHUD::ExitShopMenu()
{
	MenuSwitcher->SetActiveWidget(PromptMenu);
	OnVisible.Broadcast(this, false);
}

UREInventoryComponent* UREInteractablePointHUD::GetOwnerPlayerInventory() const
{
	if (const auto PlayerController = GetOwningPlayer())
		return Cast<UREInventoryComponent>(
			PlayerController->GetPawn()->GetComponentByClass(UREInventoryComponent::StaticClass()));

	return nullptr;
}
