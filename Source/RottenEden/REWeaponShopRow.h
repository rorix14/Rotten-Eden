// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "REWeaponShopRow.generated.h"

class UButton;
class UTextBlock;
class AREGun;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnByItemSignature, const int16&, BuyResult);

UCLASS()
class ROTTENEDEN_API UREWeaponShopRow : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetUpWeaponRow(const TSubclassOf<AREGun>& WeaponClass);

	FOnByItemSignature OnByItemEvent;

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* ButtonFailureSound;

	TSubclassOf<AREGun> WeaponToSellClass;
	
	UPROPERTY(meta = (BindWidget))
	UButton* GunBtn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GunNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GunPriceText;
	
	UPROPERTY(meta = (BindWidget))
	UButton* GunAmmoBtn;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BuyGunAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GunAmmoPriceText;

	UPROPERTY()
	USoundBase* ButtonByGunSound;
	
	UPROPERTY()
	USoundBase* ButtonByAmmoSound;

	UFUNCTION()
	void OnByWeapon();

	UFUNCTION()
    void OnByWeaponAmmo();

	UFUNCTION()
	void OnGunAmmoBtnHovered();

	UFUNCTION()
	void OnGunAmmoBtnUnHovered();

	class UREInventoryComponent* GetOwnerPlayerInventory() const;
};
