// Fill out your copyright notice in the Description page of Project Settings.


#include "REZombieAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AREZombieAIController::AREZombieAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	TargetPlayerKeyName = TEXT("TargetPlayer");
}

void AREZombieAIController::SetControllerBehaviorTree(APawn* ControlledZombie,
                                                      UBehaviorTree* ZombieBehaviorTree) const
{
	if (!ControlledZombie || !ZombieBehaviorTree) return;

	if(ZombieBehaviorTree->BlackboardAsset)
	{
		BlackboardComp->InitializeBlackboard(*ZombieBehaviorTree->BlackboardAsset);
		BlackboardComp->SetValueAsVector(TEXT("StartingPosition"), ControlledZombie->GetActorLocation());
	}
	
	BehaviorComp->StartTree(*ZombieBehaviorTree);
}

void AREZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void AREZombieAIController::OnUnPossess()
{
	Super::OnUnPossess();

	BehaviorComp->StopTree();
}

void AREZombieAIController::SetTargetEnemy(APawn* NewTarget) const
{
	if (BlackboardComp)
		BlackboardComp->SetValueAsObject(TargetPlayerKeyName, NewTarget);
}

AActor* AREZombieAIController::GetTarget() const
{
	UObject* Target = nullptr;
	if (BlackboardComp)
	{
		if((Target = BlackboardComp->GetValueAsObject(TargetPlayerKeyName)) == nullptr)			
			Target = BlackboardComp->GetValueAsObject("TargetObjective");
	
	}

	return Cast<AActor>(Target);
}
