#pragma once

#include "CoreMinimal.h"

#include "REPlayerController.h"
#include "GameFramework/Actor.h"
#include "REGun.generated.h"

class AREFPGun;

USTRUCT()
struct FWeaponAmmo
{
	GENERATED_BODY()
	
	UPROPERTY()
	int16 CurrentAmmo;
	
	UPROPERTY()
	int16 CurrentMagazineAmmo;
};

UCLASS()
class ROTTENEDEN_API AREGun : public AActor
{
	GENERATED_BODY()

public:
	AREGun();

	bool HasVisibleWeapon() const;

	void SetWeaponMeshVisibility(bool&& IsVisible) const;

	void EquipGun(APawn* ShooterCharacter, USkeletalMeshComponent* ThirdPersonMesh,
	              USkeletalMeshComponent* FirstPersonMesh);

	void StartFire();

	void EndFire();

	bool ReloadWeapon();

	void UnequipGun();

	void AddToCurrentAmmo(const int16& AmmoToAdd);

	AREFPGun* GetFPGun() const { return FPGun; }
	const TSubclassOf<AREFPGun>& GetFPGunClass() const { return FirstPersonWeaponClass; }
	int16 GetCurrentAmmo() const { return WeaponAmmo.CurrentAmmo; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	UPROPERTY(Replicated)
	AREFPGun* FPGun;

	virtual class ARETracerRound*
	CreateWeaponProjectile(USkeletalMeshComponent* AttachToMesh, const FVector& ShootEndLocation) const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireEffects(const FVector& ShootEndLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactEffects(const FVector& HitLocation, const EPhysicalSurface& SurfaceType,
	                            const FVector& ImpactEffectScale = FVector(1));

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSound(USoundBase* SoundToSend);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AREFPGun> FirstPersonWeaponClass;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponAmmo)
	FWeaponAmmo WeaponAmmo;

	float LastFiredTime;

	FTimerHandle TimerHandle_TimeBetweenShots;

	void AttachWeapon(AActor* WeaponToAttach, APawn* ShooterCharacter, USkeletalMeshComponent* MeshToAttachTo,
	                  const FName& SocketName);

	void FireGun();

	virtual bool HasShootHit(FHitResult& Hit, FVector& ShootDir);

	void UpdatePlayersGunUI(const AREPlayerController* PC, const bool& IsEquipping) const;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION()
    void OnRep_WeaponAmmo(FWeaponAmmo OldWeaponAmmo);
};
