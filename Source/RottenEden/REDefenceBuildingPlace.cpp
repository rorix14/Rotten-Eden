// Fill out your copyright notice in the Description page of Project Settings.


#include "REDefenceBuildingPlace.h"
#include "REPlayerController.h"
#include "REDefence.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AREDefenceBuildingPlace::AREDefenceBuildingPlace()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentBuiltDefenceIndex = -1;
	CurrentBuiltDefence = nullptr;
	CollisionTestBox = CreateDefaultSubobject<UBoxComponent>("Collision Box");
	CollisionTestBox->SetupAttachment(RootComponent);
}

AREDefence* AREDefenceBuildingPlace::ReturnCurrentDefence(int8 IndexValue)
{
	return buildableDefences[IndexValue]->GetDefaultObject<AREDefence>();
}

bool AREDefenceBuildingPlace::BuildDefence(TSubclassOf<AREDefence> Defence, APlayerController* DefenceOwner)
{
	UWorld* World = GetWorld();
	if (World)
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		MeshComp->SetVisibility(false);
		MeshComp->SetCastShadow(false);
		CurrentBuiltDefenceIndex = GetCurrenDefenceIndex(Defence);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += (Defence->GetDefaultObject<AREDefence>()->CollisionBox->GetScaledBoxExtent().Z);

		CurrentBuiltDefence = World->SpawnActor<AREDefence>(Defence, SpawnLocation,
		                                                    GetActorRotation(), SpawnParams);
		if (CurrentBuiltDefence)
			CurrentBuiltDefence->SetOwningPlayer(DefenceOwner);
		else
			return false;
		// if (CurrentBuiltDefence == nullptr)
		// {
		// 	//MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		// 	//MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
		// 	MeshComp->SetVisibility(true);
		// 	MeshComp->SetCastShadow(true);
		// 	CurrentBuiltDefenceIndex = -1;
		// 	return false;
		// }
		return true;
	}
	return false;
}

void AREDefenceBuildingPlace::RepairDefence() const
{
	CurrentBuiltDefence->HealDefence();
}

void AREDefenceBuildingPlace::SellDefence()
{
	CurrentBuiltDefenceIndex = -1;
	CurrentBuiltDefence->SelfDestruct();
	CurrentBuiltDefence = nullptr;
	//MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
	MeshComp->SetVisibility(true);
	MeshComp->SetCastShadow(true);
}

bool AREDefenceBuildingPlace::CanSpawn(TSubclassOf<AREDefence> Defence) const
{
	CollisionTestBox->SetBoxExtent(Defence->GetDefaultObject<AREDefence>()->CollisionBox->GetScaledBoxExtent());

	TArray<AActor*> TargetsInRange;
	CollisionTestBox->GetOverlappingActors(TargetsInRange, APawn::StaticClass());
	return TargetsInRange.Num() <= 0;
}

bool AREDefenceBuildingPlace::IsCurrentDefenceEqualToMenuIndex(int8 CurrentDefenceIndex) const
{
	return CurrentDefenceIndex == CurrentBuiltDefenceIndex;
}

bool AREDefenceBuildingPlace::IsDefenceBuilt() const
{
	return CurrentBuiltDefence && !CurrentBuiltDefence->IsDead;
}

bool AREDefenceBuildingPlace::IsDefenceAtMaxHealth() const
{
	return CurrentBuiltDefence && CurrentBuiltDefence->IsAtMaxHealth();
}

float AREDefenceBuildingPlace::GetDefenceHealthPercentage() const
{
	if(CurrentBuiltDefence)
		return CurrentBuiltDefence->DefenseHealth / CurrentBuiltDefence->DefenseMaxHealth;

		return 0;
}

void AREDefenceBuildingPlace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentBuiltDefence || CurrentBuiltDefence && CurrentBuiltDefence->IsDead)
	{
		if (HasAuthority())
			CurrentBuiltDefenceIndex = -1;
		//MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		//MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
		MeshComp->SetVisibility(true);
		MeshComp->SetCastShadow(true);
	}
}

void AREDefenceBuildingPlace::OnRep_CurrentBuildDefence()
{
	if (IsDefenceBuilt())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		MeshComp->SetVisibility(false);
		MeshComp->SetCastShadow(false);
		//UE_LOG(LogTemp, Warning, TEXT("Client has defence"));
	}
}

int8 AREDefenceBuildingPlace::GetCurrenDefenceIndex(TSubclassOf<AREDefence> Defence)
{
	for (int8 i = 0; i < buildableDefences.Num(); ++i)
	{
		if (buildableDefences[i] == Defence)
			return i;
	}

	return -1;
}

void AREDefenceBuildingPlace::OnPlayerEnterInteractableRadius(UPrimitiveComponent* OverlappedComponent,
                                                              AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                              int32 OtherBodyIndex, bool bFromSweep,
                                                              const FHitResult& SweepResult)
{
	if (CurrentPlayer == nullptr)
	{
		if (auto PC = OtherActor->GetInstigatorController<AREPlayerController>())
		{
			PC->SetClientInteractablePointVisibility(true, PromptMessage, MenuIndex, this);
			CurrentPlayer = OtherActor;
		}
	}
}

void AREDefenceBuildingPlace::OnPlayerLeaveInteractableRadius(UPrimitiveComponent* OverlappedComponent,
                                                              AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                                              int32 OtherBodyIndex)
{
	if (CurrentPlayer == OtherActor)
	{
		if (auto PC = OtherActor->GetInstigatorController<AREPlayerController>())
		{
			PC->SetClientInteractablePointVisibility(false, PromptMessage, 0, this);
			CurrentPlayer = nullptr;
		}
	}
}


void AREDefenceBuildingPlace::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AREDefenceBuildingPlace, CurrentBuiltDefenceIndex);
	DOREPLIFETIME(AREDefenceBuildingPlace, CurrentBuiltDefence);
}
