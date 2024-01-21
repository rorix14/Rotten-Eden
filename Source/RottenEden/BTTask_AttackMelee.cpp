// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackMelee.h"
#include "AIController.h"
#include "REZombieBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Kismet/GameplayStatics.h"

UBTTask_AttackMelee::UBTTask_AttackMelee()
{
	NodeName = TEXT("Do Melee Attack");
}

EBTNodeResult::Type UBTTask_AttackMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AActor* TargetActor = Cast<AActor>
		(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	
	AREZombieBase* ZombiePawn = OwnerComp.GetAIOwner()->GetPawn<AREZombieBase>();

	if (!TargetActor)
	{
		ZombiePawn->SetCanAttack(false);
		return EBTNodeResult::Failed;
	}

	/*Set animation to do the attack*/
	ZombiePawn->SetCanAttack(true);
	return EBTNodeResult::Succeeded;
}
