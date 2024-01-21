// Fill out your copyright notice in the Description page of Project Settings.


#include "REZombieMeleeAttackAnimNotify.h"

#include "REZombieBase.h"

void UREZombieMeleeAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (const auto ZombieOwner = Cast<AREZombieBase>(MeshComp->GetOwner()))
		ZombieOwner->PerformMeleeStrike();
}
