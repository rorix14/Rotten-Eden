// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "REZombieBase.generated.h"


class USoundCue;
class UREHealthComponent;

UCLASS()
class ROTTENEDEN_API AREZombieBase : public ACharacter
{
	GENERATED_BODY()

public:
	AREZombieBase(const class FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	void SetCanAttack(bool&& bHasRange) { bCanAttack = bHasRange; }

	virtual void PerformMeleeStrike();

	/*Used to set animation*/
	UFUNCTION(BlueprintCallable)
	bool GetCanAttack() const { return bCanAttack; }

	UFUNCTION(BlueprintCallable)
	uint8 GetIsDead() const { return IsDead; }

	bool GetSensedTarget() const { return bSensedTarget; }
	USoundCue* GetAttackSound() const { return SoundAttackMelee; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAudioComponent* AudioLoopComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UREHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MeleeDamage;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MeleeRange;

	/*Sets up attack animation to perform the correct damage function*/
	UPROPERTY(Replicated)
	bool bCanAttack;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health, const float& HealthDelta,
	                             const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

private:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UPawnSensingComponent* ZombieSensingComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TArray<USkeletalMesh*> ZombieMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TArray<TSubclassOf<AActor>> ZombieWeaponClasses;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SenseTimeOut;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	uint16 CreditsReward;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundPlayerNoticed;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundHunting;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundIdle;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundAttackMelee;

	bool bSensedTarget;

	float LastSeenTime;

	float LastHeardTime;

	UPROPERTY(Replicated)
	AActor* ZombieWeapon;

	/*Sets the death animation randomly, replicated for congruency between clients*/
	UPROPERTY(Replicated)
	uint8 IsDead;

	UPROPERTY(ReplicatedUsing = OnRep_MeshIndex)
	uint8 MeshIndex;

	UFUNCTION()
	void OnRep_MeshIndex();

	UFUNCTION()
	void OnSeePlayer(APawn* Pawn);

	UFUNCTION()
	void OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume);

	void SetSensedTarget(APawn* Pawn, float& LastSensedTime);

	float GetTargetRadius(AActor* Target) const;

	void PlayCharacterSound(USoundBase* CueToPlay) const;

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastUpdateAudioLoop(bool bNewSensedTarget);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
