// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REZombieBase.h"
#include "REZombieExploder.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API AREZombieExploder : public AREZombieBase
{
	GENERATED_BODY()

public:
	virtual void PerformMeleeStrike() override;

private:
	UPROPERTY(EditAnywhere, Category = "Exploder")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Exploder")
	USoundBase* ExplosionSound;

	void Explode(AActor* DamageCauser, AController* InstigatedBy);

	void ApplySelfDamage();

	virtual void OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health, const float& HealthDelta,
	                             const UDamageType* DamageType, AController* InstigatedBy,
	                             AActor* DamageCauser) override;
};
