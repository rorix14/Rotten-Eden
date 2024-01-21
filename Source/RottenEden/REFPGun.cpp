#include "REFPGun.h"
#include <Kismet/GameplayStatics.h>
#include "RottenEden.h"
#include <Components/DecalComponent.h>
#include "Particles/ParticleSystemComponent.h"

AREFPGun::AREFPGun()
{
	if ((MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"))) != nullptr)
	{
		RootComponent = MeshComp;
		MeshComp->bOnlyOwnerSee = true;
		MeshComp->CastShadow = false;
	}

	WeaponDamage = 20;
	MagazineSize = 30;
	AmmoCapacity = 100;
	RateOfFire = 400;
	bIsAutomatic = true;
	ZoomedFOV = 65.0f;
	ZoomedPosition = FVector(55.0f, -.3f, -29.0);
	ZoomedRotation = FRotator(.0f, -90.3f, .0f);
	BulletsPerShoot = 1;
	ProjectileFrequency = 7;
	BulletSpread = 2.0f;
	RecoilAccumulatedTime = 0;
	OldVertRecoilAmount = 0;
	OldHorRecoilAmount = 0;

	bReplicates = true;
}

void AREFPGun::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetVisibility(false);

	TimeBetweenShots = 60 / RateOfFire;
}

void AREFPGun::FireFPGun(USkeletalMeshComponent* AttachToMesh, const FVector& ShootEndLocation) const
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, AttachToMesh, TEXT("MuzzleSocket"), FVector::ZeroVector,
	                                       FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

	if (TracerEffect)
		if (UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), TracerEffect, AttachToMesh->GetSocketLocation(TEXT("MuzzleSocket"))))
			TracerComp->SetVectorParameter(TEXT("BeamEnd"), ShootEndLocation);
}

void AREFPGun::StopFire()
{
	RecoilAccumulatedTime = 0;
	OldVertRecoilAmount = 0;
	OldHorRecoilAmount = 0;
}

UTexture2D* AREFPGun::DoGunAimEffects(const bool& IsAiming) const
{	
	if(GunScopeUIImage && IsAiming)
		UGameplayStatics::SpawnSound2D(GetWorld(), OnBeginAimSound);
	//Return null based on wanted visibility so UI can adapt 
	return IsAiming ? GunScopeUIImage : nullptr;
}

void AREFPGun::PlayNoAmmoEffects() const
{
	UGameplayStatics::SpawnSound2D(GetWorld(), OutOfAmmoSound);
}

void AREFPGun::PlayReloadEffects() const
{
	UGameplayStatics::SpawnSound2D(GetWorld(), ReloadSound);
}

void AREFPGun::PlayEquipGunSounds(bool&& IsEquipped)
{
	UGameplayStatics::SpawnSound2D(GetWorld(), IsEquipped ? EquipGunSound : UnequipGunSound);
}

void AREFPGun::PlayImpactEffects(const FVector& HitLocation, const EPhysicalSurface SurfaceType,
                                 const FVector& ImpactEffectScale) const
{
	const FVector ShootDirectionParticles(MeshComp->GetSocketLocation(TEXT("MuzzleSocket")) - HitLocation);
	const FVector ShootDirection(HitLocation - MeshComp->GetSocketLocation(TEXT("MuzzleSocket")));

	if (SurfaceType == SURFACE_HEAD || SurfaceType == SURFACE_TORSO || SurfaceType == SURFACE_ARMS || SurfaceType ==
		SURFACE_LEGS)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CharacterImpactEffect, HitLocation,
		                                         ShootDirectionParticles.Rotation());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), CharacterImpactSound, HitLocation);
	}
	else
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GroundImpactEffect, HitLocation,
		                                         ShootDirectionParticles.Rotation(), ImpactEffectScale);
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), GroundImpactSound, HitLocation);
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleDecal, {6.5f, 6.5f, 6.5f},
		                                                                HitLocation, ShootDirection.Rotation(), 15);
		Decal->SetFadeScreenSize(0.001f);
	}
}

uint16 AREFPGun::GetDamageToApply(const EPhysicalSurface SurfaceType) const
{
	switch (SurfaceType)
	{
	case SURFACE_HEAD:
		return WeaponDamage * 4;
	case SURFACE_TORSO:
		return WeaponDamage * 0.5f;
	case SURFACE_ARMS:
		return WeaponDamage;
	case SURFACE_LEGS:
		return WeaponDamage * 2;
	default:
		return WeaponDamage;
	}
}

void AREFPGun::DoWeaponRecoil(APlayerController* WeaponOwnerController)
{
	WeaponOwnerController->ClientPlayCameraShake(FireCamShake);

	RecoilAccumulatedTime += TimeBetweenShots;
	const float NewVertAmount = VerticalRecoil->GetFloatValue(RecoilAccumulatedTime);
	const float NewHorAmount = HorizontalRecoil->GetFloatValue(RecoilAccumulatedTime);

	const FRotator ControllerRot = WeaponOwnerController->GetControlRotation();
	const FRotator Recoil(ControllerRot.Pitch + (NewVertAmount - OldVertRecoilAmount),
	                      ControllerRot.Yaw + (NewHorAmount - OldHorRecoilAmount), ControllerRot.Roll);

	WeaponOwnerController->SetControlRotation(Recoil);

	OldVertRecoilAmount = NewVertAmount;
	OldHorRecoilAmount = NewHorAmount;
}
