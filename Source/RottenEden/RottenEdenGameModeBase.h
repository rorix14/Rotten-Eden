// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "REZombieBase.h"
#include "GameFramework/GameModeBase.h"
#include "RottenEdenGameModeBase.generated.h"

/**
 * 
 */
USTRUCT()
struct FZombieParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Fields")
	TSubclassOf<AREZombieBase> ZombieRef;

	UPROPERTY(EditAnywhere, Category="Fields")
	float WaveStartSpawnPercentage;

	float SpawnPercentage;
	
	int16 EntitiesToSpawn;
};

UCLASS()
class ROTTENEDEN_API ARottenEdenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	ARottenEdenGameModeBase();

public:
	void OnZombieKilled(AActor* Killer, const uint16& Reward);

	void OnPlayerDied();

	void AddToNumberOfObjectivePlaces(int16&& ValueToAdd) { NumberOfObjectivePlaces += ValueToAdd; }

	void OnObjectivePlaceDestroyed();

	virtual void StartPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	int16 TimeBetweenWaves;

	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	float ZombieSpawnRate;

	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	float SpawnRadius;

	// Might Remove for a more sophisticated algorithm  
	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	int16 DefaultNumberOfZombiesToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Waves")
	int32 MinSpawnDistanceFromPlayer;

	UPROPERTY(EditAnywhere, Category = "Waves")
	TArray<FZombieParams> ZombiesParams;

	UPROPERTY()
	TArray<class AZombieSpawnPoint*> LevelSpawnPoints;

	int32 TotalNrOfZombiesToSpawn;

	int32 NrOfZombiesLeftToSpawn;
	//used to control the number of allowed zombies on the map
	int32 NrOfActiveZombies;

	int16 NumberOfObjectivePlaces;

	int16 WaveCount;

	FTimerHandle TimerHandle_NextWaveStart;

	FTimerHandle TimerHandle_BotSpawner;

	bool IsGameOver;

	void PrepareForNextWave();

	void StartWave();

	void SpawnZombieTimerElapsed();

	void SpawnNewZombie();

	void EndWave();

	void RestartDeadPlayers();

	void GameOver();

	FTransform GetZombieSpawnLocation();
};
