// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsObjectiveValid.h"
#include "REHealthComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_IsObjectiveValid::UBTService_IsObjectiveValid()
{
	NodeName = TEXT("Is Objecyive Valid?");
}

void UBTService_IsObjectiveValid::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// check is the objective is alive
	if (const auto ObjectiveTarget = Cast<AActor>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey())))
		if (UREHealthComponent* ObjectiveHealth = Cast<UREHealthComponent>(
			ObjectiveTarget->GetComponentByClass(UREHealthComponent::StaticClass())))
			if (!ObjectiveHealth->IsAlive(ObjectiveTarget))
				OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
}
