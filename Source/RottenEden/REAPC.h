// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REDefence.h"
#include "REAPC.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API AREAPC : public AREDefence
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
	AREAPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void ShootingTarget();
	virtual bool HasLineOfSight(FHitResult& Hit, FVector ShootDir);
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
};
