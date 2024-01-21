// Fill out your copyright notice in the Description page of Project Settings.


#include "REZombieAnimInstance.h"

#include "REZombieBase.h"

UREZombieAnimInstance::UREZombieAnimInstance()
{
	Speed = 0.0f;
	bCanAttack = false;
}

void UREZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (AActor* OwningActor = GetOwningActor())
	{
		const FVector ActorVelocity = OwningActor->GetVelocity();
		Speed = ActorVelocity.Size();

		if (const auto OwningCharacter = Cast<AREZombieBase>(OwningActor))
		{
			bCanAttack = OwningCharacter->GetCanAttack();
			if(OwningCharacter->GetIsDead() == 1)
				Death_1 = true;
			else if(OwningCharacter->GetIsDead() == 2)
				Death_2 = true;
		}
	}
}
