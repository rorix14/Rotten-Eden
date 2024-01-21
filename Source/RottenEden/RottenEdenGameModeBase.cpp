// Copyright Epic Games, Inc. All Rights Reserved.


#include "RottenEdenGameModeBase.h"

#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "REHealthComponent.h"
#include "REPlayerController.h"
#include "RottenEden.h"
#include "ZombieSpawnPoint.h"

ARottenEdenGameModeBase::ARottenEdenGameModeBase()
{
	TimeBetweenWaves = 5;
	ZombieSpawnRate = 1.5f;
	SpawnRadius = 500.0f;
	DefaultNumberOfZombiesToSpawn = 8;
	MinSpawnDistanceFromPlayer = 2000;
	NumberOfObjectivePlaces = 0;
	WaveCount = 0;
	NrOfActiveZombies = 0;
	IsGameOver = false;

	bUseSeamlessTravel = true;
}

void ARottenEdenGameModeBase::StartPlay()
{
	Super::StartPlay();

	for (TActorIterator<AZombieSpawnPoint> Iterator(GetWorld()); Iterator; ++Iterator)
		if (*Iterator)
			LevelSpawnPoints.Add(*Iterator);

	PrepareForNextWave();
}

void ARottenEdenGameModeBase::PrepareForNextWave()
{
	RestartDeadPlayers();

	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ARottenEdenGameModeBase::StartWave,
	                                TimeBetweenWaves);
}

void ARottenEdenGameModeBase::StartWave()
{
	++WaveCount;
	TotalNrOfZombiesToSpawn = 0;
	for (auto& ZombieParam : ZombiesParams)
	{
		ZombieParam.EntitiesToSpawn = FMath::RoundToInt(
			ZombieParam.WaveStartSpawnPercentage * ((3 + GetNumPlayers()) * WaveCount + DefaultNumberOfZombiesToSpawn));
		TotalNrOfZombiesToSpawn += ZombieParam.EntitiesToSpawn;
	}

	NrOfZombiesLeftToSpawn = TotalNrOfZombiesToSpawn;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ARottenEdenGameModeBase::SpawnZombieTimerElapsed,
	                                ZombieSpawnRate, true, 0.0f);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (const auto PC = Cast<AREPlayerController>(It->Get()))
			PC->NotifyWaveState(WaveCount, false);
}

void ARottenEdenGameModeBase::SpawnZombieTimerElapsed()
{
	//UE_LOG(LogTemp, Warning, TEXT("Zombies left to spawn %d"), NrOfZombiesLeftToSpawn);
	if (NrOfActiveZombies > TotalNrOfZombiesToSpawn / 2) return;

	SpawnNewZombie();
	--NrOfZombiesLeftToSpawn;
	++NrOfActiveZombies;

	if (NrOfZombiesLeftToSpawn <= 0)
		EndWave();
}

void ARottenEdenGameModeBase::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
	//UE_LOG(LogTemp, Warning, TEXT("Wave ended"));
}

void ARottenEdenGameModeBase::GameOver()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (const auto PC = Cast<AREPlayerController>(It->Get()))
			PC->ClientShowGameOverMenu();

	IsGameOver = true;
	//UE_LOG(LogTemp, Warning, TEXT("GAME OVER..."));
}

void ARottenEdenGameModeBase::RestartDeadPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (const auto PC = Cast<AREPlayerController>(It->Get()))
		{
			if (!PC->GetPawn())
				RestartPlayer(PC);

			if (WaveCount > 0)
				PC->NotifyWaveState(WaveCount, true);
		}
	}
}

void ARottenEdenGameModeBase::OnZombieKilled(AActor* Killer, const uint16& Reward)
{
	if (const auto PC = Cast<AREPlayerController>(Killer))
		PC->UpdatePlayerCredits(Reward);

	--NrOfActiveZombies;

	if (NrOfZombiesLeftToSpawn > 0 || GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart)) return;

	for (TActorIterator<ACharacter> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		if (!*Iterator || Iterator->IsPlayerControlled())
			continue;

		UREHealthComponent* HealthComp = Cast<UREHealthComponent>(
			Iterator->GetComponentByClass(UREHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
			return;
	}

	PrepareForNextWave();
}

void ARottenEdenGameModeBase::OnPlayerDied()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
			if (AActor* MyPawn = PC->GetPawn())
			{
				// Can probably change to see if the player controller has a pawn instead
				UREHealthComponent* HealthComp = Cast<UREHealthComponent>(
					MyPawn->GetComponentByClass(UREHealthComponent::StaticClass()));
				if (HealthComp && HealthComp->GetHealth() > 0.0f)
					return;
			}
	}

	if (!IsGameOver)
		GameOver();
}

void ARottenEdenGameModeBase::OnObjectivePlaceDestroyed()
{
	--NumberOfObjectivePlaces;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (const auto PC = Cast<AREPlayerController>(It->Get()))
			PC->ClientOnObjectiveDestroyedPrompt(NumberOfObjectivePlaces);

	if (!IsGameOver && NumberOfObjectivePlaces <= 0)
		GameOver();
}

void ARottenEdenGameModeBase::SpawnNewZombie()
{
	const float RandomPercentage = FMath::FRandRange(0, 1);
	float PercentageAccumulator = 0.0f;
	for (auto& ZombieParam : ZombiesParams)
	{
		ZombieParam.SpawnPercentage = static_cast<float>(ZombieParam.EntitiesToSpawn) / static_cast<float>(
			NrOfZombiesLeftToSpawn);
		if (ZombieParam.SpawnPercentage + PercentageAccumulator >= RandomPercentage)
		{
			--ZombieParam.EntitiesToSpawn;
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			GetWorld()->SpawnActor<AREZombieBase>(ZombieParam.ZombieRef, GetZombieSpawnLocation(), SpawnParams);
			return;
		}

		PercentageAccumulator += ZombieParam.SpawnPercentage;
	}

	//UE_LOG(LogTemp, Warning, TEXT("No zombie in percentage range... somthing must be wrong"));
}

FTransform ARottenEdenGameModeBase::GetZombieSpawnLocation()
{
	// add a score to each spawn point based on distance and visibility from all players
	const int32 PlayerControllerNum = GetWorld()->GetNumPlayerControllers();

	for (const auto& SpawnPoint : LevelSpawnPoints)
	{
		SpawnPoint->ResetScore();
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PC = It->Get();
			if (AActor* MyPawn = PC->GetPawn())
			{
				if (FVector::Dist(SpawnPoint->GetActorLocation(), MyPawn->GetActorLocation()) >=
					MinSpawnDistanceFromPlayer)
					SpawnPoint->AddToScore(35 / PlayerControllerNum);

				FHitResult Hit;
				FVector Dir(MyPawn->GetActorLocation() - SpawnPoint->GetActorLocation());
				GetWorld()->LineTraceSingleByChannel(Hit, SpawnPoint->GetActorLocation(), Dir * 10000,
				                                     COLLISION_WEAPON);

				APawn* HitPawn = Cast<APawn>(Hit.GetActor());
				if (!HitPawn || !HitPawn->IsPlayerControlled())
					SpawnPoint->AddToScore(65 / PlayerControllerNum);
			}
		}
	}

	// shuffle array so we accesses elements randomly 
	const int32 LastIndex = LevelSpawnPoints.Num() - 1;
	for (int32 i = 0; i <= LastIndex; ++i)
	{
		const int32 IndexToSwap = FMath::RandRange(i, LastIndex);
		if (i != IndexToSwap)
			LevelSpawnPoints.Swap(i, IndexToSwap);
	}

	// return the first spawn point that has a score higher than 60
	//if there is none just return the spawn point with the highest score 
	AZombieSpawnPoint* HighestSpawnPoint = nullptr;
	for (auto SpawnPoint : LevelSpawnPoints)
	{
		if (!HighestSpawnPoint)
			HighestSpawnPoint = SpawnPoint;

		if (HighestSpawnPoint->GetsScore() < SpawnPoint->GetsScore())
			HighestSpawnPoint = SpawnPoint;

		if (HighestSpawnPoint->GetsScore() > 60)
			break;
	}

	//Find a random position that is close to the waypoint 
	// FTransform SpawnTransform(HighestSpawnPoint ? HighestSpawnPoint->GetActorTransform() : FTransform::Identity);
	// if (HighestSpawnPoint)
	// {
	// 	FNavLocation ResultLocation;
	// 	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	// 	if (NavSystem)
	// 	{
	// 		while (!NavSystem->GetRandomReachablePointInRadius(SpawnTransform.GetLocation(), SpawnRadius,
	//                                                           ResultLocation)){}
	//
	// 		SpawnTransform.SetLocation(ResultLocation.Location);
	// 	}
	// 	
	// this works but ranges can cause them to spawn inside walls
	// do
	// {
	// 	SpawnTransform.SetLocation(
	//   SpawnTransform.GetLocation() + FVector(
	//       FMath::RandRange(-HighestSpawnPoint->GetSpawnRadius(), HighestSpawnPoint->GetSpawnRadius()),
	//       FMath::RandRange(-HighestSpawnPoint->GetSpawnRadius(), HighestSpawnPoint->GetSpawnRadius()), 0));
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("Spawn Location %s"), *SpawnTransform.GetLocation().ToString());
	// }
	//
	// while (NavSystem->ProjectPointToNavigation(SpawnTransform.GetLocation(), ResultLocation));

	//UE_LOG(LogTemp, Warning, TEXT("Result Location %s"), *ResultLocation.Location.ToString());
	//}

	return HighestSpawnPoint ? HighestSpawnPoint->GetActorTransform() : FTransform::Identity;
}
