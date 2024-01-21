// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "REPlayerMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UREPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	UREPlayerMovementComponent();
	
	virtual float GetMaxSpeed() const override;
};
