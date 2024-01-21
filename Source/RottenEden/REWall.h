// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "REDefence.h"
#include "REWall.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API AREWall : public AREDefence
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* LowWallComponent1;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* LowWallComponent2;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* WallComponent1;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* WallComponent2;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* WallComponent3;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* WallComponent4;
	UPROPERTY(VisibleAnywhere, Category = "Components")
		class UStaticMeshComponent* WallComponent5;
	// Sets default values for this pawn's properties
	AREWall();
	
};
