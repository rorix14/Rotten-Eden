// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_FindObjective.h"

#include "REHealthComponent.h"
#include "REObjectivePoint.h"
#include "REZombieAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTaskNode_FindObjective::UBTTaskNode_FindObjective()
{
	NodeName = TEXT("Find Objective");
}

EBTNodeResult::Type UBTTaskNode_FindObjective::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AREZombieAIController* ZombieController = Cast<AREZombieAIController>(OwnerComp.GetAIOwner());
	if (!ZombieController)
		return EBTNodeResult::Failed;

	TArray<AActor*> AllObjectiveLocations;
	UGameplayStatics::GetAllActorsOfClass(ZombieController, AREObjectivePoint::StaticClass(), AllObjectiveLocations);

	AActor* Objective = nullptr;
	float MinDistanceToObjective = INFINITY;
	for (auto ObjectiveLocation : AllObjectiveLocations)
	{
		if (UREHealthComponent* ObjectiveHealth = Cast<UREHealthComponent>(
			ObjectiveLocation->GetComponentByClass(UREHealthComponent::StaticClass())))
			if (ObjectiveHealth->IsAlive(ObjectiveLocation))
			{
				const float DistanceToObjective = FVector::Dist(ZombieController->GetPawn()->GetActorLocation(),
				                                                ObjectiveLocation->GetActorLocation());
				if (MinDistanceToObjective > DistanceToObjective)
				{
					MinDistanceToObjective = DistanceToObjective;
					Objective = ObjectiveLocation;
				}
			}
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), Objective);

	return Objective ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
