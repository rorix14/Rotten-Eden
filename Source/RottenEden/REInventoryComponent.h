// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "REInventoryComponent.generated.h"

class AREGun;
class AREDefence;
class AREDefenceBuildingPlace;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROTTENEDEN_API UREInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UREInventoryComponent();

	AREGun* GenerateWeaponInventory(USkeletalMeshComponent* ThirdPersonMesh, USkeletalMeshComponent* FirstPersonMesh);

	AREGun* InputChangeGun(int16&& InputValue);

	bool CanChangeGun() const { return WeaponInventory.Num() > 1; }

	void AddToHealthPack(int16&& ValueToAdd);

	bool CanConsumeHealthPack() const { return CurrentHealthPackNumber > 0; }

	bool CanBuyHealthPack() const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBuyHealthPack();

	bool CanByGun(const TSubclassOf<class AREFPGun>& FPGunClass);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBuyGun(TSubclassOf<AREGun> FPGunClass);

	bool CanByGunAmmo(const TSubclassOf<class AREFPGun>& FPGunClass);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBuyGunAmmo(TSubclassOf<AREFPGun> FPGunClass);

	bool CanBuyDefence(AREDefenceBuildingPlace* BuildingPlace, const TSubclassOf<AREDefence>& Defence, FString& Reason);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBuyDefence(AREDefenceBuildingPlace* BuildingPlace, TSubclassOf<AREDefence> Defence);

	bool CanBuyRepairDefence(AREDefenceBuildingPlace* BuildingPlace, const TSubclassOf<AREDefence>& Defence, FString& Reason);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerBuyRepairDefence(AREDefenceBuildingPlace* BuildingPlace, TSubclassOf<AREDefence> Defence);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReturnMoneySellDefence(AREDefenceBuildingPlace* BuildingPlace, TSubclassOf<AREDefence> Defence);

	void ClearInventory();

	int16 GetMedkitPrice() const { return MedkitPrice; }

protected:
	virtual void BeginPlay() override;

	void SpawnGun(USkeletalMeshComponent* ThirdPersonMesh, USkeletalMeshComponent* FirstPersonMesh,
	              FActorSpawnParameters SpawnParams, TSubclassOf<AREGun>& WeaponClass);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int16 MaxNumOfHealthPacks;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int16 MedkitPrice;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<AREGun>> WeaponClasses;

	UPROPERTY(Replicated)
	TArray<AREGun*> WeaponInventory;

	UPROPERTY()
	USkeletalMeshComponent* ThirdPersonMeshToAttach;

	UPROPERTY()
	USkeletalMeshComponent* FirstPersonMeshToAttach;

	UPROPERTY(ReplicatedUsing = OnRep_HealPackChanged)
	int16 CurrentHealthPackNumber;

	int16 CurrentWeaponIndex;

	UFUNCTION()
	void OnRep_HealPackChanged(const int16& OldHealthPackNumber);

	class AREPlayerState* GetPlayerSate() const;

	class AREPlayerController* GetPlayerController() const;

	AREGun* FindGunInInventory(const TSubclassOf<AREFPGun>& FPGunClass);

	bool HasCredits() const;
};


