// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "REInteractablePlaceBase.generated.h"

UCLASS()
class ROTTENEDEN_API AREInteractablePlaceBase : public AActor
{
	GENERATED_BODY()

public:
	AREInteractablePlaceBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "InteractablePlace")
		FName PromptMessage;

	UPROPERTY(EditDefaultsOnly, Category = "InteractablePlace")
		uint8 MenuIndex;


	UPROPERTY(VisibleAnywhere, Category = "Components")
		UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		class USphereComponent* SphereComp;

private:
	UFUNCTION()
	virtual void OnPlayerEnterInteractableRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                             const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnPlayerLeaveInteractableRadius(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
