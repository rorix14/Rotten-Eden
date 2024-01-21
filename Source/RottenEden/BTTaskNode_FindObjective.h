// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTaskNode_FindObjective.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UBTTaskNode_FindObjective : public  UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTaskNode_FindObjective();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
