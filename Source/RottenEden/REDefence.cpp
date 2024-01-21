// Fill out your copyright notice in the Description page of Project Settings.


#include "REDefence.h"

#include "AIController.h"
#include "Components/BoxComponent.h"
#include "NavigationSystem.h"
#include "REHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AREDefence::AREDefence()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>("Collision Box");
	RootComponent = CollisionBox;
	HealthComp = CreateDefaultSubobject<UREHealthComponent>(TEXT("HealthComp"));

	AudioLoopComp = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineSound"));
	AudioLoopComp->bAutoActivate = false;
	AudioLoopComp->bAutoDestroy = false;
	AudioLoopComp->SetupAttachment(RootComponent);
	IsDead = false;
	TimeToDie = 0.5f;

	for (int i = 0; i < 10; ++i)
	{
		const FName LocationName = TEXT("EffectLocationBiggerName") + i;
		if (auto EffectLocation = CreateDefaultSubobject<USceneComponent>(LocationName))
		{
			EffectLocation->SetupAttachment(RootComponent);
			EffectPointsLocations.Add(EffectLocation);
		}
	}
}

// Called when the game starts or when spawned
void AREDefence::BeginPlay()
{
	Super::BeginPlay();
	if (HealthComp)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &AREDefence::OnHealthChanged);
		DefenseMaxHealth = HealthComp->GetMaxHealth();
		DefenseHealth = HealthComp->GetHealth();
	}

	DamageEffectsReference.Reserve(EffectPointsLocations.Num());
	IndexesUsed.Reserve(EffectPointsLocations.Num());
}

void AREDefence::OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health, const float& HealthDelta,
                                 const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!OwningHealthComp) return;
	//UE_LOG(LogTemp, Warning, TEXT("Toke damage health: %s"), *FString::SanitizeFloat(Health));

	if (bUseDamageEffects)
	{
		const float Percentage = 1 - Health / OwningHealthComp->GetMaxHealth();
		const int NumberToSpawn = FMath::RoundToInt(Percentage * EffectPointsLocations.Num() - IndexesUsed.Num());
		//UE_LOG(LogTemp, Warning, TEXT("Number to sapwn: %d"), NumberToSpawn);

		if (NumberToSpawn > 0)
		{
			for (int i = 0; i < NumberToSpawn; ++i)
			{
				int16 EffectPlaceIndex;
				do
					EffectPlaceIndex = FMath::RandRange(0, EffectPointsLocations.Num() - 1);
				while (IndexesUsed.Contains(EffectPlaceIndex));

				IndexesUsed.Add(EffectPlaceIndex);
				DamageEffectsReference.Add(UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
					Percentage > 0.6f ? FireEffect : SmokeEffect,
					EffectPointsLocations[EffectPlaceIndex]->GetComponentLocation(),
					FRotator::ZeroRotator, FVector(0.6f)));
			}
		}
		else
		{
			for (int i = 0; i > NumberToSpawn; --i)
			{
				IndexesUsed.Pop(false);
				UParticleSystemComponent* DamageEffect = DamageEffectsReference.Pop(false);
				if (DamageEffect)
					DamageEffect->DestroyComponent();
			}
		}
	}

	DefenseHealth = Health;
	if (Health > 0 && HealthDelta < 0)
	{
		MulticastSoundEffects(GetHitSound);
	}
	else if (Health <= 0)
	{
		for (auto Effect : DamageEffectsReference)
			Effect->DestroyComponent();

		if (HasAuthority())
		{
			IsDead = true;
			MulticastSoundEffects(DestructionSound);
			MulticastDeathEffect(DeathEffect);
			DetachFromControllerPendingDestroy();
			GetWorldTimerManager().SetTimer(TimerHandle_TimeToDie, this, &AREDefence::OnDeath, TimeToDie);
		}
	}
}

void AREDefence::OnDeath()
{
	Destroy();
}

void AREDefence::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(EndPlayReason == EEndPlayReason::Destroyed)
	{
		for (auto Element : DamageEffectsReference)
			Element->DestroyComponent();
		
		DamageEffectsReference.Empty();
		IndexesUsed.Empty();
	}

}

void AREDefence::MulticastFireEffects_Implementation(USceneComponent* AttachToMesh, const FName& SocketName,
                                                     const FVector& EffectScale,
                                                     UParticleSystem* OptionalMuzzleEffect,
                                                     USoundBase* OptionalShotSound)
{
	UGameplayStatics::SpawnEmitterAttached(OptionalMuzzleEffect ? OptionalMuzzleEffect : MuzzleEffect, AttachToMesh,
	                                       SocketName, FVector::ZeroVector,
	                                       FRotator::ZeroRotator, EffectScale, EAttachLocation::SnapToTarget);

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), OptionalShotSound ? OptionalShotSound : ShotSound,
	                                       AttachToMesh->GetSocketLocation(SocketName));
}

void AREDefence::MulticastSoundEffects_Implementation(USoundBase* SoundCue)
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundCue, GetActorLocation());
}

void AREDefence::MulticastDeathEffect_Implementation(UParticleSystem* Effect)
{
	FRotator EffectRotation;
	EffectRotation.Add(0, 90, 0);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Effect, GetActorLocation(),
	                                         FRotator::ZeroRotator, DeathEffectScale);
}

void AREDefence::MulticastImpactEffects_Implementation(USceneComponent* MeshComp, const FName& SocketName,
                                                       const FVector& HitLocation, const FVector& ImpactEffectScale,
                                                       UParticleSystem* OptionalCharacterImpactEffect,
                                                       USoundBase* OptionalCharacterImpactSound)
{
	const FVector ShootDirectionParticles(MeshComp->GetSocketLocation(SocketName) - HitLocation);
	//const FVector ShootDirection(HitLocation - MeshComp->GetSocketLocation(SocketName));

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(), OptionalCharacterImpactEffect ? OptionalCharacterImpactEffect : CharacterImpactEffect, HitLocation,
		ShootDirectionParticles.Rotation(), ImpactEffectScale);
	UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(), OptionalCharacterImpactSound ? OptionalCharacterImpactSound : CharacterImpactSound, HitLocation);
}

bool AREDefence::IsAtMaxHealth() const
{
	return HealthComp && HealthComp->GetMaxHealth() == HealthComp->GetHealth();
}

// Called every frame
void AREDefence::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FString AREDefence::ReturnName()
{
	return DefenceName;
}

FString AREDefence::ReturnStats()
{
	FString statList = "Cost: " + FString::FromInt(DefencePrice) + "    Health: " + DefenceMaxHealthString +
		"\nDamage: " + DamageStat + "   Shoots Every: " + FireRate;
	return statList;
}


void AREDefence::SelfDestruct()
{
	Destroy();
}

void AREDefence::HealDefence() const
{
	HealthComp->Heal(HealthComp->GetMaxHealth() * 0.20);
}

void AREDefence::SetOwningPlayer(APlayerController* PlayerController) const
{
	if (auto AICon = GetController<AAIController>())
		AICon->SetOwner(PlayerController);
}

void AREDefence::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AREDefence, currentTarget);
	DOREPLIFETIME(AREDefence, IsDead);
}
