// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REGun.h"
#include "REGrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API AREGrenadeLauncher : public AREGun
{
	GENERATED_BODY()

	AREGrenadeLauncher();

	UPROPERTY(EditDefaultsOnly, Category = "GrenadeLauncher")
	int16 ExplosionRange;

	virtual bool HasShootHit(FHitResult& Hit, FVector& ShootDir) override;

	virtual class ARETracerRound*
	CreateWeaponProjectile(USkeletalMeshComponent* AttachToMesh, const FVector& ShootEndLocation) const override;

	UFUNCTION()
	void OnGrenadeDestroyed(AActor* Actor, EEndPlayReason::Type EndPlayReason);
};
