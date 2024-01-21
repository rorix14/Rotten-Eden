// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REDefence.h"
#include "RETank.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API ARETank : public AREDefence
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UPoseableMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USphereComponent* RangeSphere;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* CollisionBox1;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UBoxComponent* CollisionBox2;
	// Sets default values for this pawn's properties
	ARETank();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void ShootingTargetCannon();
	virtual void ShootingTargetGun();
	virtual bool HasLineOfSight(FHitResult& Hit, FVector ShootDir);
	FTimerHandle TimerHandle_TimeBetweenShotsCannon;
	FTimerHandle TimerHandle_TimeBetweenShotsGun;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float defenseRangeCannon;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float defenseRangeGun;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float GunDamage;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float CannonDamage;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float shootingIntervalCannon;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float shootingIntervalGun;
	UPROPERTY(Replicated)
		class AREZombieBase* currentCannonTarget;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector GunEffectScale;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector GunImpactEffectScale;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector CannonEffectScale;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector CannonImpactEffectScale;
	UPROPERTY(EditAnywhere, Category = "Visual")
		UParticleSystem* CannonMuzzleEffect;
	UPROPERTY(EditAnywhere, Category = "Visual")
		UParticleSystem* CannonCharacterImpactEffect;
	UPROPERTY(EditAnywhere, Category = "Visual")
		USoundBase* CannonCharacterImpactSound;
	UPROPERTY(EditAnywhere, Category = "Visual")
		USoundBase* CannonShotSound;
};
