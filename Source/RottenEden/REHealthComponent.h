// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "REHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UREHealthComponent*, HealthComp, const float&,
                                             Health, const float&, HealthDelta, const UDamageType*, DamageType,
                                             AController*, InstigatedBy, AActor*, DamageCauser);

UENUM()
enum class ETeamType : uint8
{
	PLAYER,
	DEFENCE,
	ZOMBIE,
	NONE
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ROTTENEDEN_API UREHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UREHealthComponent();

	UPROPERTY()
	FOnHealthChangedSignature OnHealthChanged;

	bool IsFriendly(AActor* ActorA, AActor* ActorB) const;

	bool Heal(const float& HealAmount);

	bool IsAlive(AActor* ActorToCheck) const;

	float GetMaxHealth() const { return DefaultHealth; }
	float GetHealth() const { return Health; }
	USoundBase* GetGetHurtSound() const { return GetHurtSound; }
	ETeamType GetTeamType() const { return Team; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "HealthComponent")
	ETeamType Team;

	UPROPERTY(EditAnywhere, Category = "HealthComponent")
	TSubclassOf<UCameraShake> GetHurtCamShake;

	UPROPERTY(EditAnywhere, Category = "HealthComponent")
	float DefaultHealth;

	UPROPERTY(EditAnywhere, Category = "HealthComponent")
	USoundBase* UseMedkitSound;

	UPROPERTY(EditAnywhere, Category = "HealthComponent")
	USoundBase* GetHurtSound;

	UPROPERTY(EditAnywhere, Category = "HealthComponent")
	USoundBase* GetKilledSound;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	                         class AController* InstigatedBy, AActor* DamageCauser);
};
