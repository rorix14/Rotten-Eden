// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "ZombieSpawnPoint.generated.h"

/**
 * 
 */
UCLASS()
class ROTTENEDEN_API AZombieSpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

	AZombieSpawnPoint();

public:
	float GetsScore() const { return Score; }

	void ResetScore() { Score = 0; }

	void AddToScore(float&& Value) { Score += Value; }

	float GetSpawnRadius() const { return SpawnRadius; }

private:
	UPROPERTY(EditAnywhere, Category="Spawn Point")
	float SpawnRadius;

	float Score;
};
