// Fill out your copyright notice in the Description page of Project Settings.
#include "REHealthComponent.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/GameplayStatics.h>

UREHealthComponent::UREHealthComponent()
{
	DefaultHealth = 150.0f;
	Health = 150.0f;
	SetIsReplicatedByDefault(true);
}

void UREHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UREHealthComponent::HandleTakeAnyDamage);

	Health = DefaultHealth;
	OnHealthChanged.Broadcast(this, Health, DefaultHealth, nullptr, nullptr, nullptr);
}

bool UREHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB) const
{
	if (!ActorA || !ActorB) return true;

	const auto HealthCompA = Cast<UREHealthComponent>(ActorA->GetComponentByClass(StaticClass()));
	const auto HealthCompB = Cast<UREHealthComponent>(ActorB->GetComponentByClass(StaticClass()));

	if (!HealthCompA || !HealthCompB) return true;

	return HealthCompA->GetTeamType() == HealthCompB->GetTeamType();
}

bool UREHealthComponent::Heal(const float& HealAmount)
{
	if (HealAmount <= 0.f || Health <= 0.f || Health == DefaultHealth) return false;

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	const auto PC = GetOwner() ? GetOwner()->GetInstigatorController() : nullptr;
	if (PC && PC->IsLocalController())
		UGameplayStatics::SpawnSound2D(GetWorld(), UseMedkitSound);
	//UE_LOG(LogTemp, Warning, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, HealAmount, nullptr, nullptr, nullptr);
	return true;
}

bool UREHealthComponent::IsAlive(AActor* ActorToCheck) const
{
	if (!ActorToCheck) return false;

	const auto HealthComp = Cast<UREHealthComponent>(ActorToCheck->GetComponentByClass(StaticClass()));
	if (!HealthComp) return false;

	return HealthComp->GetHealth() > 0;
}

void UREHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
                                             class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || Health <= 0) return;

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	if (const auto MyOwner = Cast<APawn>(GetOwner()))
	{
		if (Health > 0)
		{
			if (auto PC = MyOwner->GetController<APlayerController>())
			{
				PC->ClientPlayCameraShake(GetHurtCamShake);

				if (PC->IsLocalController())
					UGameplayStatics::PlaySound2D(GetWorld(), GetHurtSound);
			}
		}
		else
			UGameplayStatics::SpawnSoundAttached(GetKilledSound, MyOwner->GetRootComponent(), NAME_None,
			                                     FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
	}

	OnHealthChanged.Broadcast(this, Health, -Damage, DamageType, InstigatedBy, DamageCauser);
}


void UREHealthComponent::OnRep_Health(float OldHealth)
{
	if (Health > 0)
	{
		if (GetOwnerRole() == ROLE_AutonomousProxy)
		{
			if (OldHealth > Health)
				UGameplayStatics::PlaySound2D(GetWorld(), GetHurtSound);
			else
				UGameplayStatics::PlaySound2D(GetWorld(), UseMedkitSound);
		}
	}
	else
		UGameplayStatics::SpawnSoundAttached(GetKilledSound, GetOwner()->GetRootComponent(), NAME_None,
		                                     FVector::ZeroVector, EAttachLocation::SnapToTarget, true);


	OnHealthChanged.Broadcast(this, Health, Health - OldHealth, nullptr, nullptr, nullptr);
}

void UREHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UREHealthComponent, Health);
}
