// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "REZombieAIController.generated.h"

class AREZombieBase;

UCLASS()
class ROTTENEDEN_API AREZombieAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AREZombieAIController();

	void SetControllerBehaviorTree(APawn* ControlledZombie, class UBehaviorTree* ZombieBehaviorTree) const;
	
	void SetTargetEnemy(APawn* NewTarget) const;
	
	AActor* GetTarget() const;

private:
	UPROPERTY()
	class UBehaviorTreeComponent* BehaviorComp;

	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	virtual void OnPossess(class APawn* InPawn) override;

	virtual void OnUnPossess() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetPlayerKeyName;
};
