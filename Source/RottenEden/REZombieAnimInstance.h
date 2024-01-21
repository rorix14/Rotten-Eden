// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "REZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UREZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	UREZombieAnimInstance();

	public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bCanAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool Death_1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool Death_2;
};
