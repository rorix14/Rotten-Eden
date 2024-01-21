// Fill out your copyright notice in the Description page of Project Settings.


#include "REInventoryComponent.h"
#include <Net/UnrealNetwork.h>
#include "REFPGun.h"
#include "REGun.h"
#include "REPlayerState.h"
#include "REDefence.h"
#include "REDefenceBuildingPlace.h"
#include "REInteractablePointHUD.h"

UREInventoryComponent::UREInventoryComponent()
{
	MaxNumOfHealthPacks = 3;
	MedkitPrice = 20;
	SetIsReplicatedByDefault(true);
}

void UREInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealthPackNumber = MaxNumOfHealthPacks;
}

AREGun* UREInventoryComponent::GenerateWeaponInventory(USkeletalMeshComponent* ThirdPersonMesh,
                                                       USkeletalMeshComponent* FirstPersonMesh)
{
	ThirdPersonMeshToAttach = ThirdPersonMesh;
	FirstPersonMeshToAttach = FirstPersonMesh;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	WeaponInventory.Reserve(WeaponClasses.Num());
	for (TSubclassOf<AREGun>& WeaponClass : WeaponClasses)
		SpawnGun(ThirdPersonMeshToAttach, FirstPersonMeshToAttach, SpawnParams, WeaponClass);

	return WeaponInventory.Num() > 0 ? WeaponInventory[0] : nullptr;
}

AREGun* UREInventoryComponent::InputChangeGun(int16&& InputValue)
{
	// can have a better algorithm 
	if (WeaponInventory.Num() <= 1)
		return WeaponInventory[CurrentWeaponIndex];

	CurrentWeaponIndex += InputValue;
	CurrentWeaponIndex = CurrentWeaponIndex < 0 ? static_cast<int16>(WeaponInventory.Num()) - 1 : CurrentWeaponIndex;
	return WeaponInventory[CurrentWeaponIndex % WeaponInventory.Num()];
}

bool UREInventoryComponent::CanBuyHealthPack() const
{
	if (const auto PS = GetPlayerSate())
		return PS->GetPlayerCredits() >= MedkitPrice && CurrentHealthPackNumber < MaxNumOfHealthPacks;

	return false;
}

void UREInventoryComponent::ServerBuyHealthPack_Implementation()
{
	if (const auto PC = GetPlayerController())
		PC->UpdatePlayerCredits(-MedkitPrice);

	AddToHealthPack(1);
}

bool UREInventoryComponent::ServerBuyHealthPack_Validate()
{
	return HasCredits();
}

void UREInventoryComponent::AddToHealthPack(int16&& ValueToAdd)
{
	const int16 OldHealthPackNumber = CurrentHealthPackNumber;
	CurrentHealthPackNumber = FMath::Clamp(static_cast<int16>(CurrentHealthPackNumber + ValueToAdd),
	                                       static_cast<int16>(0), MaxNumOfHealthPacks);

	OnRep_HealPackChanged(OldHealthPackNumber);
}

void UREInventoryComponent::OnRep_HealPackChanged(const int16& OldHealthPackNumber)
{
	if (const auto PC = GetPlayerController())
		PC->UpdateMedkitUI(OldHealthPackNumber > CurrentHealthPackNumber);
}

bool UREInventoryComponent::CanByGun(const TSubclassOf<AREFPGun>& FPGunClass)
{
	if (const auto PS = GetPlayerSate())
		return PS->GetPlayerCredits() >= FPGunClass->GetDefaultObject<AREFPGun>()->GetGunPrice() &&
			!FindGunInInventory(FPGunClass);

	return false;
}

void UREInventoryComponent::ServerBuyGun_Implementation(TSubclassOf<AREGun> FPGunClass)
{
	if (const auto PC = GetPlayerController())
		PC->UpdatePlayerCredits(
			-FPGunClass->GetDefaultObject<AREGun>()->GetFPGunClass()->GetDefaultObject<AREFPGun>()->GetGunPrice());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnGun(ThirdPersonMeshToAttach, FirstPersonMeshToAttach, SpawnParams, FPGunClass);
}

bool UREInventoryComponent::ServerBuyGun_Validate(TSubclassOf<AREGun> FPGunClass)
{
	return HasCredits();
}

void UREInventoryComponent::SpawnGun(USkeletalMeshComponent* ThirdPersonMesh, USkeletalMeshComponent* FirstPersonMesh,
                                     FActorSpawnParameters SpawnParams, TSubclassOf<AREGun>& WeaponClass)
{
	auto Gun = GetWorld()->SpawnActor<AREGun>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator,
	                                          SpawnParams);
	Gun->EquipGun(Cast<APawn>(GetOwner()), ThirdPersonMesh, FirstPersonMesh);
	WeaponInventory.Add(Gun);
}

bool UREInventoryComponent::CanByGunAmmo(const TSubclassOf<AREFPGun>& FPGunClass)
{
	if (const auto PS = GetPlayerSate())
	{
		AREGun* FoundGun(FindGunInInventory(FPGunClass));

		return PS->GetPlayerCredits() >= FPGunClass->GetDefaultObject<AREFPGun>()->GetGunAmmoPrice() && FoundGun &&
			FoundGun->GetCurrentAmmo() + FoundGun->GetFPGun()->GetMagazineSize() <=
			FoundGun->GetFPGun()->GetAmmoCapacity();
	}

	return false;
}

void UREInventoryComponent::ServerBuyGunAmmo_Implementation(TSubclassOf<AREFPGun> FPGunClass)
{
	if (AREGun* FoundGun = FindGunInInventory(FPGunClass))
	{
		FoundGun->AddToCurrentAmmo(FoundGun->GetFPGun()->GetMagazineSize());
		if (const auto PC = GetPlayerController())
			PC->UpdatePlayerCredits(-FoundGun->GetFPGun()->GetGunAmmoPrice());
	}
}

bool UREInventoryComponent::ServerBuyGunAmmo_Validate(TSubclassOf<AREFPGun> FPGunClass)
{
	return HasCredits();
}

bool UREInventoryComponent::CanBuyDefence(AREDefenceBuildingPlace* BuildingPlace,
                                          const TSubclassOf<AREDefence>& Defence, FString& Reason)
{
	if (const auto PS = GetPlayerSate())
	{
		if (PS->GetPlayerCredits() >= Defence->GetDefaultObject<AREDefence>()->DefencePrice)
		{
			if (BuildingPlace->CanSpawn(Defence))
			{
				Reason = "Defence Built";
				return true;
			}
			else
			{
				Reason = "Player and /or enemies obstructing defence construction";
			}
		}
		else
		{
			Reason = "Not enough money";
		}
	}

	return false;
}

void UREInventoryComponent::ServerBuyDefence_Implementation(AREDefenceBuildingPlace* BuildingPlace,
                                                            TSubclassOf<AREDefence> Defence)
{
	if (const auto PC = GetPlayerController())
		if (BuildingPlace->BuildDefence(Defence, PC))
			PC->UpdatePlayerCredits(-Defence->GetDefaultObject<AREDefence>()->DefencePrice);
}

bool UREInventoryComponent::ServerBuyDefence_Validate(AREDefenceBuildingPlace* BuildingPlace,
                                                      TSubclassOf<AREDefence> Defence)
{
	return HasCredits();
}

bool UREInventoryComponent::CanBuyRepairDefence(AREDefenceBuildingPlace* BuildingPlace,
                                                const TSubclassOf<AREDefence>& Defence, FString& Reason)
{
	if (const auto PS = GetPlayerSate())
	{
		if (PS->GetPlayerCredits() >= Defence->GetDefaultObject<AREDefence>()->DefencePrice * 0.2f)
		{
			if (!BuildingPlace->IsDefenceAtMaxHealth())
			{
				Reason = TEXT("Defence Repaired");
				return true;
			}
			Reason = TEXT("Defence at max health");
		}
		else
			Reason = "Not enough money";
	}

	return false;
}

void UREInventoryComponent::ServerBuyRepairDefence_Implementation(AREDefenceBuildingPlace* BuildingPlace,
                                                                  TSubclassOf<AREDefence> Defence)
{
	if (const auto PC = GetPlayerController())
		PC->UpdatePlayerCredits(-Defence->GetDefaultObject<AREDefence>()->DefencePrice * 0.20);

	BuildingPlace->RepairDefence();
}

bool UREInventoryComponent::ServerBuyRepairDefence_Validate(AREDefenceBuildingPlace* BuildingPlace,
                                                            TSubclassOf<AREDefence> Defence)
{
	return HasCredits();
}

void UREInventoryComponent::ServerReturnMoneySellDefence_Implementation(AREDefenceBuildingPlace* BuildingPlace,
                                                                        TSubclassOf<AREDefence> Defence)
{
	if (const auto PC = GetPlayerController())
		PC->UpdatePlayerCredits(
			Defence->GetDefaultObject<AREDefence>()->DefencePrice * BuildingPlace->GetDefenceHealthPercentage());

	BuildingPlace->SellDefence();
}

bool UREInventoryComponent::ServerReturnMoneySellDefence_Validate(AREDefenceBuildingPlace* BuildingPlace,
                                                                  TSubclassOf<AREDefence> Defence) { return true; }

AREGun* UREInventoryComponent::FindGunInInventory(const TSubclassOf<AREFPGun>& FPGunClass)
{
	for (const auto Gun : WeaponInventory)
		if (FPGunClass == Gun->GetFPGunClass())
			return Gun;

	return nullptr;
}

bool UREInventoryComponent::HasCredits() const
{
	if (const auto PS = GetPlayerSate())
		return  PS->GetPlayerCredits() >= 0;

	return false;
}

AREPlayerState* UREInventoryComponent::GetPlayerSate() const
{
	if (const auto MyOwner = Cast<APawn>(GetOwner()))
		return MyOwner->GetPlayerState<AREPlayerState>();

	return nullptr;
}

AREPlayerController* UREInventoryComponent::GetPlayerController() const
{
	if (const auto MyOwner = Cast<APawn>(GetOwner()))
		return MyOwner->GetController<AREPlayerController>();

	return nullptr;
}

void UREInventoryComponent::ClearInventory()
{
	for (AREGun* Weapon : WeaponInventory)
	{
		Weapon->GetFPGun()->Destroy();
		Weapon->Destroy();
	}
}

void UREInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UREInventoryComponent, CurrentHealthPackNumber, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UREInventoryComponent, WeaponInventory, COND_OwnerOnly);
}
