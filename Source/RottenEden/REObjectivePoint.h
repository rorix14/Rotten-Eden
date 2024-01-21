// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "REObjectivePoint.generated.h"

UCLASS()
class ROTTENEDEN_API AREObjectivePoint : public AActor
{
	GENERATED_BODY()

public:
	AREObjectivePoint();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UREHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAudioComponent* AudioLoopComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UWidgetComponent* HealthBarWidgetComp;

	UPROPERTY(EditAnywhere, Category = "ObjectivePoint")
	USoundBase* GettingHitSound;

	UPROPERTY(VisibleAnywhere)
	TArray<USceneComponent*> EffectPointsLocations;

	UPROPERTY(EditDefaultsOnly)
	int16 NumberOfEffectLocations;

	TArray<int16> IndexesUsed;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	UParticleSystem* SmokeEffect;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	UParticleSystem* FireEffect;

	UFUNCTION()
	void OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health, const float& HealthDelta,
	                             const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
