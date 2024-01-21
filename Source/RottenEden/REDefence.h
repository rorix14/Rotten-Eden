// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "REDefence.generated.h"

class UREHealthComponent;

UCLASS()
class ROTTENEDEN_API AREDefence : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AREDefence();
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* CollisionBox;

	UFUNCTION()
	void SelfDestruct();

	UFUNCTION()
	void HealDefence() const;

	void SetOwningPlayer(APlayerController* PlayerController) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UREHealthComponent* HealthComp;

	UFUNCTION()
	void OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health, const float& HealthDelta,
	                     const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	UPROPERTY(Replicated)
	class AREZombieBase* currentTarget;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireEffects(USceneComponent* AttachToMesh, const FName& SocketName, const FVector& EffectScale,
	                          UParticleSystem* OptionalMuzzleEffect = nullptr, USoundBase* OptionalShotSound = nullptr);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSoundEffects(USoundBase* SoundCue);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeathEffect(UParticleSystem* Effect);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastImpactEffects(USceneComponent* MeshComp, const FName& SocketName, const FVector& HitLocation,
	                            const FVector& ImpactEffectScale = FVector(1),
	                            UParticleSystem* OptionalCharacterImpactEffect = nullptr,
	                            USoundBase* OptinalCharacterImpactSound = nullptr);

	FTimerHandle TimerHandle_TimeToDie;

	void OnDeath();

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	float TimeToDie;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	bool bUseDamageEffects;

	UPROPERTY(VisibleAnywhere)
	TArray<USceneComponent*> EffectPointsLocations;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	UParticleSystem* SmokeEffect;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	UParticleSystem* FireEffect;

	TArray<int16> IndexesUsed;

	TArray<UParticleSystemComponent*> DamageEffectsReference;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool IsDead;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gameplay")
	float DefenseHealth;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Gameplay")
	float DefenseMaxHealth;
	UPROPERTY(EditAnywhere, Category = "Visual")
	UParticleSystem* MuzzleEffect;
	UPROPERTY(EditAnywhere, Category = "Visual")
	UParticleSystem* CharacterImpactEffect;
	UPROPERTY(EditAnywhere, Category = "Visual")
	UParticleSystem* DeathEffect;
	UPROPERTY(EditAnywhere, Category = "Visual")
	USoundBase* CharacterImpactSound;
	UPROPERTY(EditAnywhere, Category = "Visual")
	USoundBase* ShotSound;
	UPROPERTY(EditAnywhere, Category = "Visual")
	USoundBase* DestructionSound;
	UPROPERTY(EditAnywhere, Category = "Visual")
	USoundBase* GetHitSound;
	UPROPERTY(EditAnywhere, Category = "Visual")
	FVector DeathEffectScale;
	UPROPERTY(EditAnywhere, Category = "Info")
	FString DefenceMaxHealthString;
	UPROPERTY(EditAnywhere, Category = "Info")
	FString DefenceName;
	UPROPERTY(EditAnywhere, Category = "Info")
	int DefencePrice;
	UPROPERTY(EditAnywhere, Category = "Info")
	FString DamageStat;
	UPROPERTY(EditAnywhere, Category = "Info")
	FString FireRate;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAudioComponent* AudioLoopComp;
	UPROPERTY(EditAnywhere, Category = "Info")
	UTexture2D* DefenceUIImage;

	bool IsAtMaxHealth() const;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual FString ReturnName();
	virtual FString ReturnStats();
};
