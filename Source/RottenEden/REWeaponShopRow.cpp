// Fill out your copyright notice in the Description page of Project Settings.


#include "REWeaponShopRow.h"
#include "REFPGun.h"
#include "REGun.h"
#include "REInventoryComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UREWeaponShopRow::SetUpWeaponRow(const TSubclassOf<AREGun>& WeaponClass)
{
	WeaponToSellClass = WeaponClass;
	AREFPGun* Weapon = WeaponToSellClass->GetDefaultObject<AREGun>()->GetFPGunClass()->GetDefaultObject<AREFPGun>();
	if (!Weapon) return;

	GunBtn->WidgetStyle.Normal.SetResourceObject(Weapon->GetGunUIImage());
	GunBtn->WidgetStyle.Hovered.SetResourceObject(Weapon->GetGunUIImage());
	GunBtn->WidgetStyle.Pressed.SetResourceObject(Weapon->GetGunUIImage());
	GunNameText->SetText(FText::FromName(Weapon->GetGunName()));
	BuyGunAmmoText->SetText(FText::FromString(Weapon->GetGunName().ToString() + " Ammo"));
	GunPriceText->SetText(FText::FromString("$" + FString::FromInt(Weapon->GetGunPrice())));
	GunAmmoPriceText->SetText(FText::FromString("$" + FString::FromInt(Weapon->GetGunAmmoPrice())));
	// Experiment with other sounds
	ButtonByGunSound = Weapon->GetGunBuySound();
	ButtonByAmmoSound = Weapon->GetGunAmmoBuySound();

	GunBtn->OnClicked.AddDynamic(this, &UREWeaponShopRow::OnByWeapon);
	GunAmmoBtn->OnClicked.AddDynamic(this, &UREWeaponShopRow::OnByWeaponAmmo);
	GunAmmoBtn->OnHovered.AddDynamic(this, &UREWeaponShopRow::OnGunAmmoBtnHovered);
	GunAmmoBtn->OnUnhovered.AddDynamic(this, &UREWeaponShopRow::OnGunAmmoBtnUnHovered);
}

void UREWeaponShopRow::OnByWeapon()
{
	if (const auto PlayerInventory = GetOwnerPlayerInventory())
	{
		const TSubclassOf<AREFPGun> Weapon = WeaponToSellClass->GetDefaultObject<AREGun>()->GetFPGunClass();
		if (PlayerInventory->CanByGun(Weapon))
		{
			PlayerInventory->ServerBuyGun(WeaponToSellClass);
			OnByItemEvent.Broadcast(Weapon->GetDefaultObject<AREFPGun>()->GetGunPrice());
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonByGunSound);
		}
		else
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonFailureSound);
	}
}

void UREWeaponShopRow::OnByWeaponAmmo()
{
	if (const auto PlayerInventory = GetOwnerPlayerInventory())
	{
		const TSubclassOf<AREFPGun> Weapon = WeaponToSellClass->GetDefaultObject<AREGun>()->GetFPGunClass();
		if (PlayerInventory->CanByGunAmmo(Weapon))
		{
			// move by function inside the can buy function
			PlayerInventory->ServerBuyGunAmmo(Weapon);
			OnByItemEvent.Broadcast(Weapon->GetDefaultObject<AREFPGun>()->GetGunAmmoPrice());
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonByAmmoSound);
		}
		else
			UGameplayStatics::SpawnSound2D(GetWorld(), ButtonFailureSound);
	}
}

void UREWeaponShopRow::OnGunAmmoBtnHovered()
{
	BuyGunAmmoText->SetColorAndOpacity(FLinearColor::White);
}

void UREWeaponShopRow::OnGunAmmoBtnUnHovered()
{
	BuyGunAmmoText->SetColorAndOpacity(FLinearColor::Gray);
}

UREInventoryComponent* UREWeaponShopRow::GetOwnerPlayerInventory() const
{
	if (const auto PlayerController = GetOwningPlayer())
		return Cast<UREInventoryComponent>(
			PlayerController->GetPawn()->GetComponentByClass(UREInventoryComponent::StaticClass()));

	return nullptr;
}
