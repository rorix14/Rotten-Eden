// Fill out your copyright notice in the Description page of Project Settings.


#include "REGrenadeLauncher.h"

#include "DrawDebugHelpers.h"
#include "REFPGun.h"
#include "RETracerRound.h"
#include "Kismet/GameplayStatics.h"

AREGrenadeLauncher::AREGrenadeLauncher()
{
	ExplosionRange = 400;
}

bool AREGrenadeLauncher::HasShootHit(FHitResult& Hit, FVector& ShootDir)
{
	if (AController* OwnerController = GetOwner()->GetInstigatorController())
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
		const float HalfRad = FMath::DegreesToRadians(FPGun->GetBulletSpread());
		ShootDir = FMath::VRandCone(EyeRotation.Vector(), HalfRad, HalfRad);
		const FVector End = EyeLocation + ShootDir * 10000;
		MulticastFireEffects(End);
	}

	return false;
}

ARETracerRound* AREGrenadeLauncher::CreateWeaponProjectile(USkeletalMeshComponent* AttachToMesh,
                                                           const FVector& ShootEndLocation) const
{
	auto Grenade = Super::CreateWeaponProjectile(AttachToMesh, ShootEndLocation);
	if (Grenade && HasAuthority())
		Grenade->OnEndPlay.AddDynamic(this, &AREGrenadeLauncher::OnGrenadeDestroyed);

	return Grenade;
}

void AREGrenadeLauncher::OnGrenadeDestroyed(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, FPGun->GetDamageToApply(SurfaceType_Default),
		                                    Actor->GetActorLocation(),
		                                    ExplosionRange, nullptr, IgnoredActors,
		                                    this, GetInstigatorController(), true);

		/*Use for debugging range */
		//DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), ExplosionRange, 12, FColor::Red, false, 2.0f, 0, 1.5f);  
		MulticastImpactEffects(Actor->GetActorLocation(), SurfaceType_Default, FVector(1.5f));
	}
}
