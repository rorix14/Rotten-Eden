#include "REGun.h"
#include "REFPGun.h"
#include "REPlayerController.h"
#include <Net/UnrealNetwork.h>
#include <TimerManager.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include <Kismet/GameplayStatics.h>
#include "RottenEden.h"
#include "RETracerRound.h"
#include "Particles/ParticleSystemComponent.h"

AREGun::AREGun()
{
	if ((MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"))) != nullptr)
	{
		RootComponent = MeshComp;
		MeshComp->bOwnerNoSee = true;
		MeshComp->bCastHiddenShadow = true;
	}

	LastFiredTime = 0;
	bReplicates = true;
}

void AREGun::BeginPlay()
{
	Super::BeginPlay();

	MeshComp->SetVisibility(false);
	MeshComp->SetCastShadow(false);
}

bool AREGun::HasVisibleWeapon() const
{
	if (FPGun)
		return MeshComp->IsVisible() && FPGun->GetWeaponMesh()->IsVisible();

	return false;
}

void AREGun::SetWeaponMeshVisibility(bool&& IsVisible) const
{
	if (FPGun)
	{
		MeshComp->SetVisibility(IsVisible);
		MeshComp->SetCastShadow(IsVisible);
		FPGun->GetWeaponMesh()->SetVisibility(IsVisible);

		const auto PC = GetOwner()->GetInstigatorController<AREPlayerController>();
		if (IsVisible && PC && PC->IsLocalController())
		{
			FPGun->PlayEquipGunSounds(true);
			UpdatePlayersGunUI(PC, true);
		}
	}
}

void AREGun::EquipGun(APawn* ShooterCharacter, USkeletalMeshComponent* ThirdPersonMesh,
                      USkeletalMeshComponent* FirstPersonMesh)
{
	AttachWeapon(this, ShooterCharacter, ThirdPersonMesh, TEXT("Hand_RSocket"));

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if ((FPGun = GetWorld()->SpawnActor<AREFPGun>(FirstPersonWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator,
	                                              SpawnParams)) != nullptr)
	{
		AttachWeapon(FPGun, ShooterCharacter, FirstPersonMesh, TEXT("TestFPWeapon"));
		WeaponAmmo = {FPGun->GetAmmoCapacity(), FPGun->GetMagazineSize()};
	}
}

void AREGun::AttachWeapon(AActor* WeaponToAttach, APawn* ShooterCharacter, USkeletalMeshComponent* MeshToAttachTo,
                          const FName& SocketName)
{
	if (!WeaponToAttach) return;

	WeaponToAttach->AttachToComponent(MeshToAttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                                  SocketName);
	WeaponToAttach->SetOwner(ShooterCharacter);
	WeaponToAttach->SetInstigator(ShooterCharacter);
}

void AREGun::AddToCurrentAmmo(const int16& AmmoToAdd)
{
	WeaponAmmo.CurrentAmmo = FMath::Clamp(static_cast<int16>(WeaponAmmo.CurrentAmmo + AmmoToAdd), static_cast<int16>(0),
	                                      FPGun->GetAmmoCapacity());

	if (HasVisibleWeapon())
		UpdatePlayersGunUI(nullptr, false);
}

bool AREGun::ReloadWeapon()
{
	if (!FPGun || WeaponAmmo.CurrentAmmo == 0 || WeaponAmmo.CurrentMagazineAmmo == FPGun->GetMagazineSize())
		return false;

	int16 ClipAmmoUsed = FPGun->GetMagazineSize() - WeaponAmmo.CurrentMagazineAmmo;
	WeaponAmmo.CurrentMagazineAmmo += WeaponAmmo.CurrentAmmo >= ClipAmmoUsed ? ClipAmmoUsed : WeaponAmmo.CurrentAmmo;
	WeaponAmmo.CurrentAmmo = FMath::Clamp(WeaponAmmo.CurrentAmmo, static_cast<int16>(0),
	                                      WeaponAmmo.CurrentAmmo -= ClipAmmoUsed);

	// might just call the on rep function
	UpdatePlayersGunUI(nullptr, false);

	const auto PC = GetOwner() ? GetOwner()->GetInstigatorController() : nullptr;
	if (PC && PC->IsLocalController())
		FPGun->PlayReloadEffects();
	
	return true;
}

void AREGun::UnequipGun()
{
	if (FPGun)
	{
		EndFire();
		FPGun->PlayEquipGunSounds(false);
	}
}

void AREGun::StartFire()
{
	if (FPGun)
	{
		const float FirstDelay = FMath::Max(LastFiredTime + FPGun->GetTimeBetweenShots() - GetWorld()->TimeSeconds,
		                                    0.0f);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AREGun::FireGun,
		                                FPGun->GetTimeBetweenShots(), FPGun->IsAutomatic(), FirstDelay);
	}
}

void AREGun::EndFire()
{
	if (FPGun)
	{
		FPGun->StopFire();
		GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
	}
}

void AREGun::FireGun()
{
	if (!FPGun) return;

	if (WeaponAmmo.CurrentMagazineAmmo <= 0)
	{
		FPGun->PlayNoAmmoEffects();
		return;
	}

	ServerFire();

	LastFiredTime = GetWorld()->TimeSeconds;

	if (const auto PC = GetOwner()->GetInstigatorController<AREPlayerController>())
		PC->PlayCrosshairAnimation();
}

void AREGun::ServerFire_Implementation()
{
	--WeaponAmmo.CurrentMagazineAmmo;
	UpdatePlayersGunUI(nullptr, false);

	if (auto MyPawn = GetInstigator())
		MyPawn->MakeNoise(1.0f, MyPawn, MyPawn->GetActorLocation());

	MulticastSound(FPGun->GetShootSound());

	FHitResult Hit;
	FVector ShootDir;
	for (int i = 0; i < FPGun->GetBulletsPerShoot(); ++i)
	{
		if (HasShootHit(Hit, ShootDir))
		{
			MulticastImpactEffects(Hit.Location, Hit.PhysMaterial->SurfaceType);

			if (FPGun)
			{
				const uint16 ActualDamage = FPGun->GetDamageToApply(Hit.PhysMaterial->SurfaceType);
				Hit.GetActor()->TakeDamage(ActualDamage, FPointDamageEvent(ActualDamage, Hit, ShootDir, nullptr),
				                           GetOwner()->GetInstigatorController(), this);
			}
		}
	}
}

bool AREGun::ServerFire_Validate() { return true; }

bool AREGun::HasShootHit(FHitResult& Hit, FVector& ShootDir)
{
	if (AController* OwnerController = GetOwner()->GetInstigatorController())
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
		const float HalfRad = FMath::DegreesToRadians(FPGun->GetBulletSpread());
		ShootDir = FMath::VRandCone(EyeRotation.Vector(), HalfRad, HalfRad);
		const FVector End = EyeLocation + ShootDir * 10000;

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());
		Params.bTraceComplex = true;
		Params.bReturnPhysicalMaterial = true;
		const bool HasHit = GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, End, COLLISION_WEAPON, Params);

		MulticastFireEffects(HasHit ? Hit.Location : End);
		return HasHit;
	}

	return false;
}

void AREGun::MulticastFireEffects_Implementation(const FVector& ShootEndLocation)
{
	const auto PC = GetOwner() ? GetOwner()->GetInstigatorController<APlayerController>() : nullptr;
	if (!PC || !PC->IsLocalController())
	{
		CreateWeaponProjectile(MeshComp, ShootEndLocation);
		FPGun->FireFPGun(MeshComp, ShootEndLocation);
	}
	else
	{
		CreateWeaponProjectile(FPGun->GetWeaponMesh(), ShootEndLocation);
		FPGun->FireFPGun(FPGun->GetWeaponMesh(), ShootEndLocation);
		FPGun->DoWeaponRecoil(PC);
	}
}

void AREGun::MulticastImpactEffects_Implementation(const FVector& HitLocation, const EPhysicalSurface& SurfaceType,
                                                   const FVector& ImpactEffectScale)
{
	if (FPGun)
		FPGun->PlayImpactEffects(HitLocation, SurfaceType, ImpactEffectScale);
}

ARETracerRound* AREGun::CreateWeaponProjectile(USkeletalMeshComponent* AttachToMesh,
                                               const FVector& ShootEndLocation) const
{
	if (!FPGun) return nullptr;

	if (FMath::RandRange(0, 10) <= FPGun->GetProjectileFrequency())
	{
		const FVector ShootDirection(ShootEndLocation - AttachToMesh->GetSocketLocation(TEXT("MuzzleSocket")));
		return GetWorld()->SpawnActor<ARETracerRound>(FPGun->GetShootProjectile(), {
			                                              ShootDirection.Rotation(),
			                                              AttachToMesh->GetSocketLocation(TEXT("MuzzleSocket")),
			                                              {1, 1, 1}
		                                              });
	}

	return nullptr;
}

void AREGun::OnRep_WeaponAmmo(FWeaponAmmo OldWeaponAmmo)
{
	if (!HasVisibleWeapon()) return;

	if (OldWeaponAmmo.CurrentAmmo > WeaponAmmo.CurrentAmmo)
		FPGun->PlayReloadEffects();

	UpdatePlayersGunUI(nullptr, false);
}

void AREGun::UpdatePlayersGunUI(const AREPlayerController* PC, const bool& IsEquipping) const
{
	if (!PC)
		if (const AActor* OwnerActor = GetOwner())
			PC = OwnerActor->GetInstigatorController<AREPlayerController>();

	if (PC)
		PC->UpdateAmmoUI(WeaponAmmo.CurrentMagazineAmmo, WeaponAmmo.CurrentAmmo, IsEquipping, FPGun->GetGunUIImage());
}

void AREGun::MulticastSound_Implementation(USoundBase* SoundToSend)
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundToSend, GetActorLocation());
}

void AREGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AREGun, FPGun);

	DOREPLIFETIME_CONDITION(AREGun, WeaponAmmo, COND_OwnerOnly);
}
