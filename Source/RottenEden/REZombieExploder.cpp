// Fill out your copyright notice in the Description page of Project Settings.


#include "REZombieExploder.h"
#include "REHealthComponent.h"
#include "Kismet/GameplayStatics.h"

void AREZombieExploder::Explode(AActor* DamageCauser, AController* InstigatedBy)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());

	GetMesh()->SetVisibility(false, true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority())
	{
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, MeleeDamage, GetActorLocation(), MeleeRange, nullptr, IgnoredActors,
		                                    DamageCauser, InstigatedBy, true);
		/*Creates a debug sphere representing explosion size*/
		// DrawDebugSphere(GetWorld(), GetActorLocation(), MeleeRange, 12, FColor::Red,
		//                 false, 2.0f, 0, 1.5f);
	}
}

void AREZombieExploder::ApplySelfDamage()
{
	UGameplayStatics::ApplyDamage(this, HealthComp->GetMaxHealth(), GetController(), this, nullptr);
}

void AREZombieExploder::OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health,
                                        const float& HealthDelta, const UDamageType* DamageType,
                                        AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0)
		Explode(DamageCauser, InstigatedBy);

	Super::OnHealthChanged(OwningHealthComp, Health, HealthDelta, DamageType, InstigatedBy, DamageCauser);
}

void AREZombieExploder::PerformMeleeStrike()
{
	if (IsLocallyControlled())
		ApplySelfDamage();
	
	bCanAttack = true;
}
