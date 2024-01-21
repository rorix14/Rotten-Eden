// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REInteractablePlaceBase.h"
#include "REDefenceBuildingPlace.generated.h"

class AREDefence;

UCLASS()
class ROTTENEDEN_API AREDefenceBuildingPlace : public AREInteractablePlaceBase
{
	GENERATED_BODY()
	

	public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	AREDefenceBuildingPlace();

	class AREDefence* ReturnCurrentDefence(int8 InputValue);

	UFUNCTION()
	bool BuildDefence(TSubclassOf<AREDefence> Defence, APlayerController* DefenceOwner);

	UFUNCTION()
	void RepairDefence() const;

	UFUNCTION()
	void SellDefence();

	UFUNCTION()
	bool CanSpawn(TSubclassOf<AREDefence> Defence) const;

	UFUNCTION()
	bool IsCurrentDefenceEqualToMenuIndex(int8 CurrentDefenceIndex) const;

	UFUNCTION()
	bool IsDefenceBuilt() const;

	bool IsDefenceAtMaxHealth() const;

	float GetDefenceHealthPercentage() const;

	int32 ReturnBuildableDefenceArraySize() const { return buildableDefences.Num();}

	TSubclassOf<class AREDefence> GetDefenceClass(int16 Index) {return buildableDefences[Index];}

	private:
	UFUNCTION()
	void OnRep_CurrentBuildDefence();

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	TArray<TSubclassOf<class AREDefence>> buildableDefences;

	UPROPERTY()
	AActor* CurrentPlayer;

	UPROPERTY(Replicated)
	int8 CurrentBuiltDefenceIndex;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentBuildDefence)
	AREDefence* CurrentBuiltDefence;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* CollisionTestBox;

	int8 GetCurrenDefenceIndex(TSubclassOf<AREDefence> Defence);

	virtual void OnPlayerEnterInteractableRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
												UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
												const FHitResult& SweepResult) override;

	virtual void OnPlayerLeaveInteractableRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
												UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

};
