#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "REFPGun.generated.h"

class ARETracerRound;

UCLASS()
class ROTTENEDEN_API AREFPGun : public AActor
{
	GENERATED_BODY()

public:
	AREFPGun();

	USkeletalMeshComponent* GetWeaponMesh() const { return MeshComp; }

	void FireFPGun(USkeletalMeshComponent* AttachToMesh, const FVector& ShootEndLocation) const;

	void StopFire();

	void DoWeaponRecoil(APlayerController* WeaponOwnerController);

	UTexture2D* DoGunAimEffects(const bool& IsAiming) const;

	void PlayNoAmmoEffects() const;

	void PlayImpactEffects(const FVector& HitLocation, EPhysicalSurface SurfaceType,
	                       const FVector& ImpactEffectScale) const;

	void PlayReloadEffects() const;

	void PlayEquipGunSounds(bool&& IsEquipped);

	uint16 GetDamageToApply(EPhysicalSurface SurfaceType) const;

	int16 GetAmmoCapacity() const { return AmmoCapacity; }
	int16 GetMagazineSize() const { return MagazineSize; }
	bool IsAutomatic() const { return bIsAutomatic; }
	float GetTimeBetweenShots() const { return TimeBetweenShots; }
	float GetZoomedFOV() const { return ZoomedFOV; }
	uint8 GetBulletsPerShoot() const { return BulletsPerShoot; }
	float GetBulletSpread() const { return BulletSpread; }
	FVector GetZoomedPosition() const { return ZoomedPosition; }
	FRotator GetZoomedRotation() const { return ZoomedRotation; }
	uint16 GetProjectileFrequency() const { return ProjectileFrequency; }
	TSubclassOf<ARETracerRound> GetShootProjectile() const { return ShootProjectile; }
	USoundBase* GetShootSound() const { return ShootSound; }
	UTexture2D* GetGunUIImage() const { return GunUIImage; }
	FName GetGunName() const {return GunName;}
	int16 GetGunPrice() const { return GunPrice; }
	int16 GetGunAmmoPrice() const { return GunAmmoPrice; }
	USoundBase* GetGunAmmoBuySound() const { return GunAmmoBuySound; }
	USoundBase* GetGunBuySound() const { return EquipGunSound; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComp;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	uint16 WeaponDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int16 AmmoCapacity;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int16 MagazineSize;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool bIsAutomatic;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FVector ZoomedPosition;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FRotator ZoomedRotation;

	UPROPERTY(EditAnywhere, Category= "Weapon")
	uint8 BulletsPerShoot;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BulletSpread;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UCurveFloat* HorizontalRecoil;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UCurveFloat* VerticalRecoil;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	uint16 ProjectileFrequency;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<ARETracerRound> ShootProjectile;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UParticleSystem* MuzzleEffect;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	UParticleSystem* TracerEffect;
	
	UPROPERTY(EditAnywhere, Category = "Weapon")
	UParticleSystem* GroundImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UParticleSystem* CharacterImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* ShootSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* GroundImpactSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* CharacterImpactSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UMaterialInterface* BulletHoleDecal;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* OutOfAmmoSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* EquipGunSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* UnequipGunSound;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UTexture2D* GunUIImage;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	UTexture2D* GunScopeUIImage;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	USoundBase* OnBeginAimSound;

	UPROPERTY(EditAnywhere, Category = "Shop")
	FName GunName;
	
	UPROPERTY(EditAnywhere, Category = "Shop")
	uint16 GunPrice;

	UPROPERTY(EditAnywhere, Category = "Shop")
	uint16 GunAmmoPrice;

	UPROPERTY(EditAnywhere, Category = "Shop")
	USoundBase* GunAmmoBuySound;

	float RecoilAccumulatedTime, OldVertRecoilAmount, OldHorRecoilAmount;
};
