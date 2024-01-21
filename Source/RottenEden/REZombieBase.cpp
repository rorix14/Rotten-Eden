// Fill out your copyright notice in the Description page of Project Settings.


#include "REZombieBase.h"
#include "REHealthComponent.h"
#include "REZombieAIController.h"
#include "REZombieMovementComponent.h"
#include "RottenEden.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/PawnSensingComponent.h"
#include "ProfilingDebugging/CookStats.h"
#include "Sound/SoundCue.h"
#include <Net/UnrealNetwork.h>
#include "RottenEdenGameModeBase.h"
#include "Components/PoseableMeshComponent.h"


AREZombieBase::AREZombieBase(const class FObjectInitializer& ObjectInitializer): Super(
	/* Override the movement class from the base class to our own class to support multiple speeds */
	ObjectInitializer.SetDefaultSubobjectClass<UREZombieMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	AudioLoopComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ZombieLoopedSoundComp"));
	AudioLoopComp->bAutoActivate = false;
	AudioLoopComp->bAutoDestroy = false;
	AudioLoopComp->SetupAttachment(RootComponent);

	ZombieSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	ZombieSensingComp->SetPeripheralVisionAngle(60.0f);
	ZombieSensingComp->SightRadius = 2000;
	ZombieSensingComp->bOnlySensePlayers = false;

	HealthComp = CreateDefaultSubobject<UREHealthComponent>(TEXT("HealthComp"));

	GetCapsuleComponent()->bFillCollisionUnderneathForNavmesh = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	bSensedTarget = false;
	bCanAttack = false;
	LastSeenTime = 0.0f;
	LastHeardTime = 0.0f;
	SenseTimeOut = 2.5f;
	CreditsReward = 10;
	MeleeRange = 350.0f;
	MeleeDamage = 30.0f;
	IsDead = 0;
}

void AREZombieBase::BeginPlay()
{
	Super::BeginPlay();

	if (const auto AIController = GetController<AREZombieAIController>())
	{
		MeshIndex = FMath::RandRange(0, ZombieMeshes.Num() - 1);
		OnRep_MeshIndex();

		AIController->SetControllerBehaviorTree(this, BehaviorTree);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (ZombieWeaponClasses.Num() > 0 && (ZombieWeapon = GetWorld()->SpawnActor<AActor>(
			ZombieWeaponClasses[FMath::RandRange(0, ZombieWeaponClasses.Num() - 1)], FVector::ZeroVector,
			FRotator::ZeroRotator, SpawnParams)) != nullptr)
		{
			ZombieWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			                                TEXT("Hand_RSocket"));
		}
	}

	if (ZombieSensingComp)
	{
		ZombieSensingComp->OnSeePawn.AddDynamic(this, &AREZombieBase::OnSeePlayer);
		ZombieSensingComp->OnHearNoise.AddDynamic(this, &AREZombieBase::OnHearNoise);
	}

	if (HealthComp)
		HealthComp->OnHealthChanged.AddDynamic(this, &AREZombieBase::OnHealthChanged);

	BroadcastUpdateAudioLoop(bSensedTarget);
}

void AREZombieBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSensedTarget && LastSeenTime > SenseTimeOut && LastHeardTime > SenseTimeOut)
		if (const auto AIController = GetController<AREZombieAIController>())
		{
			BroadcastUpdateAudioLoop(false);
			bSensedTarget = false;
			AIController->SetTargetEnemy(nullptr);
		}

	LastSeenTime += DeltaTime;
	LastHeardTime += DeltaTime;
}

void AREZombieBase::OnSeePlayer(APawn* Pawn)
{
	SetSensedTarget(Pawn, LastSeenTime);
}

void AREZombieBase::OnHearNoise(APawn* PawnInstigator, const FVector& Location, float Volume)
{
	SetSensedTarget(PawnInstigator, LastHeardTime);
}

void AREZombieBase::SetSensedTarget(APawn* Pawn, float& LastSensedTime)
{
	const auto AIController = GetController<AREZombieAIController>();

	if (AIController && Pawn && HealthComp->IsAlive(Pawn) && !HealthComp->IsFriendly(this, Pawn))
	{
		AIController->SetTargetEnemy(Pawn);
		LastSensedTime = 0;
		if (!bSensedTarget)
			BroadcastUpdateAudioLoop(true);
		bSensedTarget = true;
	}
}

void AREZombieBase::PerformMeleeStrike()
{
	if (const auto AIController = GetController<AREZombieAIController>())
		if (AActor* HitTarget = AIController->GetTarget())
		{
			if (FVector::DistXY(GetActorLocation(), HitTarget->GetActorLocation()) <= MeleeRange +
				GetTargetRadius(HitTarget))
			{
				FPointDamageEvent DmgEvent;
				DmgEvent.Damage = MeleeDamage;
				HitTarget->TakeDamage(DmgEvent.Damage, DmgEvent, GetController(), this);
			}
		}

	bCanAttack = false;
}

float AREZombieBase::GetTargetRadius(AActor* Target) const
{
	if (const auto TargetMesh = Cast<UStaticMeshComponent>(
        Target->GetComponentByClass(UStaticMeshComponent::StaticClass())))
        	return TargetMesh->GetStaticMesh()->GetBounds().SphereRadius * TargetMesh->GetComponentScale().Y;

	if(const auto TargetMesh = Cast<UPoseableMeshComponent>(Target->GetComponentByClass(UPoseableMeshComponent::StaticClass())))
		return TargetMesh->SkeletalMesh->GetBounds().SphereRadius * TargetMesh->GetComponentScale().Y;
	
	return 0;
}

void AREZombieBase::OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health, const float& HealthDelta,
                                    const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (InstigatedBy && !bSensedTarget)
		OnSeePlayer(InstigatedBy->GetPawn());

	if (Health <= 0)
	{
		if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
		{
			CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		}

		if(USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
			MeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
		}

		if (UCharacterMovementComponent* CharacterComp = GetCharacterMovement())
		{
			CharacterComp->StopMovementImmediately();
			CharacterComp->DisableMovement();
			CharacterComp->SetComponentTickEnabled(false);
		}

		if (AudioLoopComp)
			AudioLoopComp->Stop();

		if (HasAuthority())
		{
			const auto Bot = Cast<AAIController>(InstigatedBy);
			if (const auto GameMode = GetWorld()->GetAuthGameMode<ARottenEdenGameModeBase>())
				GameMode->OnZombieKilled(Bot? Bot->GetOwner() : InstigatedBy, CreditsReward);

			IsDead = FMath::RandRange(1, 2);
			DetachFromControllerPendingDestroy();
			SetLifeSpan(15.0f);
		}
	}
	else
		PlayCharacterSound(OwningHealthComp->GetGetHurtSound());
}

void AREZombieBase::PlayCharacterSound(USoundBase* CueToPlay) const
{
	UGameplayStatics::SpawnSoundAttached(CueToPlay, RootComponent, NAME_None,
	                                     FVector::ZeroVector, EAttachLocation::SnapToTarget, true);
}

void AREZombieBase::BroadcastUpdateAudioLoop_Implementation(bool bNewSensedTarget)
{
	if (!AudioLoopComp) return;

	if (bNewSensedTarget && !bSensedTarget)
	{
		PlayCharacterSound(SoundPlayerNoticed);
		AudioLoopComp->SetSound(SoundHunting);
		AudioLoopComp->Play();
	}
	else
	{
		AudioLoopComp->SetSound(SoundIdle);
		AudioLoopComp->Play();
	}
}

void AREZombieBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority() && ZombieWeapon && EndPlayReason == EEndPlayReason::Destroyed)
		ZombieWeapon->Destroy();
}

void AREZombieBase::OnRep_MeshIndex()
{
	GetMesh()->SetSkeletalMesh(ZombieMeshes[MeshIndex]);
}

void AREZombieBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AREZombieBase, ZombieWeapon, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AREZombieBase, bCanAttack, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AREZombieBase, IsDead, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AREZombieBase, MeshIndex, COND_SimulatedOnly);
}

/*Set up Ragdoll physics, currently seams to be having bugs...*/
// GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
//
// SetActorEnableCollision(true);
//
// GetMesh()->SetAllBodiesSimulatePhysics(true);
// GetMesh()->SetSimulatePhysics(true);
// GetMesh()->WakeAllRigidBodies();
// GetMesh()->bBlendPhysics = true;
