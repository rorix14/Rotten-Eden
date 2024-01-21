// Fill out your copyright notice in the Description page of Project Settings.


#include "REPlayerMovementComponent.h"

#include "REShooterCharacter.h"

UREPlayerMovementComponent::UREPlayerMovementComponent()
{
	JumpZVelocity = 480;
	NavAgentProps.bCanJump = true;
}

float UREPlayerMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const auto PlayerCharacter  = Cast<AREShooterCharacter>(PawnOwner);
	if (PlayerCharacter)
	{
		// Slow down if player is aiming down sights.
		if (PlayerCharacter->CheckForState(EPlayerAnimState::WantsToZoom))
		{
			MaxSpeed /= 2.0f;
		}
		else if(PlayerCharacter->GetWantsToSprint())
		{
			MaxSpeed *= 1.5f;
		}
	}

	return MaxSpeed;
}
