// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REDefence.h"
#include "RESniperTower.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API ARESniperTower : public AREDefence
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* TowerMeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USkeletalMeshComponent* SniperMeshComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USphereComponent* RangeSphere;
	// Sets default values for this pawn's properties
	ARESniperTower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void ShootingTarget();
	virtual bool HasLineOfSight(FHitResult& Hit, FVector ShootDir);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	FTimerHandle TimerHandle_TimeBetewenShots;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float defenseRange;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float damage;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		float shootingInterval;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector EffectScale;
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		FVector ImpactEffectScale;
	UPROPERTY(EditAnywhere, Category = "Visual")
		TSubclassOf<AActor> WeaponClass;
	UPROPERTY(Replicated)
		AActor* DefenceWeapon;
	UPROPERTY()
		USkeletalMeshComponent* WeaponMesh;
};
