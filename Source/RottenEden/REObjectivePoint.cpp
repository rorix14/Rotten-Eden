// Fill out your copyright notice in the Description page of Project Settings.


#include "REObjectivePoint.h"
#include "REHealthComponent.h"
#include "RottenEdenGameModeBase.h"
#include "Components/AudioComponent.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

AREObjectivePoint::AREObjectivePoint()
{
	if ((MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"))) != nullptr)
		RootComponent = MeshComp;

	NumberOfEffectLocations = 20;

	for (int i = 0; i < NumberOfEffectLocations; ++i)
	{
		const FName LocationName = TEXT("EffectLocationBiggerName") + i;
		if (auto EffectLocation = CreateDefaultSubobject<USceneComponent>(LocationName))
		{
			EffectLocation->SetupAttachment(MeshComp);
			EffectPointsLocations.Add(EffectLocation);
		}
	}

	if ((AudioLoopComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ZombieLoopedSoundComp"))) != nullptr)
	{
		AudioLoopComp->bAutoActivate = false;
		AudioLoopComp->bAutoDestroy = false;
		AudioLoopComp->SetupAttachment(RootComponent);
	}

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComp"));
	HealthComp = CreateDefaultSubobject<UREHealthComponent>(TEXT("HealthComp"));

	bReplicates = true;
}

void AREObjectivePoint::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComp)
		HealthComp->OnHealthChanged.AddDynamic(this, &AREObjectivePoint::OnHealthChanged);

	if (AudioLoopComp)
	{
		AudioLoopComp->SetVolumeMultiplier(0);
		AudioLoopComp->Play();
	}

	if (const auto GameMode = GetWorld()->GetAuthGameMode<ARottenEdenGameModeBase>())
		GameMode->AddToNumberOfObjectivePlaces(1);

	IndexesUsed.Reserve(EffectPointsLocations.Num());
}

void AREObjectivePoint::OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health,
                                        const float& HealthDelta, const UDamageType* DamageType,
                                        AController* InstigatedBy, AActor* DamageCauser)
{
	if (!OwningHealthComp) return;

	const float Percentage = 1 - Health / OwningHealthComp->GetMaxHealth();
	const int NumberToSpawn = FMath::RoundToInt(Percentage * EffectPointsLocations.Num() - IndexesUsed.Num());
	for (int i = 0; i < NumberToSpawn; ++i)
	{
		int16 EffectPlaceIndex;
		do
			EffectPlaceIndex = FMath::RandRange(0, EffectPointsLocations.Num() - 1);
		while (IndexesUsed.Contains(EffectPlaceIndex));

		IndexesUsed.Add(EffectPlaceIndex);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Percentage > 0.6f ? FireEffect : SmokeEffect,
		                                         EffectPointsLocations[EffectPlaceIndex]->GetComponentLocation(),
		                                         FRotator::ZeroRotator, FVector(1.5));
	}

	if (AudioLoopComp)
		AudioLoopComp->SetVolumeMultiplier(Percentage + 0.5f);

	if (UProgressBar* HealthBarWidget = HealthBarWidgetComp
		                                    ? HealthBarWidgetComp->GetUserWidgetObject()
			                                      ? Cast<UProgressBar>(
				                                      HealthBarWidgetComp->GetUserWidgetObject()->GetWidgetFromName(
					                                      TEXT("HealthBar")))
			                                      : nullptr
		                                    : nullptr)
		HealthBarWidget->SetPercent(1 - Percentage);

	UGameplayStatics::SpawnSoundAtLocation(this, GettingHitSound, GetActorLocation());

	if (Health <= 0)
		if (const auto GameMode = GetWorld()->GetAuthGameMode<ARottenEdenGameModeBase>())
			GameMode->OnObjectivePlaceDestroyed();
}
