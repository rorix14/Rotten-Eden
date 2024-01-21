// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsObjectiveValid.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API UBTService_IsObjectiveValid : public UBTService_BlackboardBase
{
	GENERATED_BODY()

	UBTService_IsObjectiveValid();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
