// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AttackMelee.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UBTTask_AttackMelee : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_AttackMelee();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
