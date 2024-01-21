// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "REZombieMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UREZombieMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	virtual float GetMaxSpeed() const override;
	
};
