// Fill out your copyright notice in the Description page of Project Settings.


#include "REZombieMovementComponent.h"

#include "REZombieBase.h"

float UREZombieMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const auto ZombieOwner = Cast<AREZombieBase>(PawnOwner);
	if (ZombieOwner)
	{
		// Slow down during if it does no have a target.
		if (!ZombieOwner->GetSensedTarget())
		{
			MaxSpeed /= 2.0f;
		}
	}

	return MaxSpeed;
}
