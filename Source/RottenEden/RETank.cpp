// Fill out your copyright notice in the Description page of Project Settings.


#include "RETank.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "REZombieBase.h"
#include "Components/PoseableMeshComponent.h"
#include "RottenEden.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/AudioComponent.h"

ARETank::ARETank()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RangeSphere = CreateDefaultSubobject<USphereComponent>("Range Sphere");
	MeshComponent = CreateDefaultSubobject<UPoseableMeshComponent>("Mesh Component");
	CollisionBox1 = CreateDefaultSubobject<UBoxComponent>("Collision Box 1");
	CollisionBox2 = CreateDefaultSubobject<UBoxComponent>("Collision Box 2");
	RangeSphere->SetupAttachment(RootComponent);
	MeshComponent->SetupAttachment(RootComponent);
	CollisionBox1->SetupAttachment(RootComponent);
	CollisionBox2->SetupAttachment(RootComponent);
}

void ARETank::BeginPlay()
{
	Super::BeginPlay();

	AudioLoopComp->Play();

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShotsCannon, this, &ARETank::ShootingTargetCannon, shootingIntervalCannon, true);
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShotsGun, this, &ARETank::ShootingTargetGun, shootingIntervalGun, true);
	}
}

void ARETank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Rotate Cannon
	if (currentCannonTarget != nullptr)
	{
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = currentCannonTarget->GetActorLocation();
		FVector Dir = (TargetLoc - MyLoc);
		Dir.Normalize();
		FRotator cannonRotation = (MeshComponent->GetBoneRotationByName("Turret", EBoneSpaces::WorldSpace));
		FRotator newRotation = FMath::Lerp(cannonRotation, FRotationMatrix::MakeFromX(FVector(Dir.X, Dir.Y, 0)).Rotator(), 0.05f);
		MeshComponent->SetBoneRotationByName("Turret", newRotation, EBoneSpaces::WorldSpace);
	}

	//Rotate Gun
	if (currentTarget != nullptr)
	{
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = currentTarget->GetActorLocation();
		FVector Dir = (TargetLoc - MyLoc);
		Dir.Normalize();
		FRotator gunRotation = MeshComponent->GetBoneRotationByName("Gun", EBoneSpaces::WorldSpace);
		FRotator newRotation = FMath::Lerp(gunRotation, FRotationMatrix::MakeFromX(FVector(Dir.X, Dir.Y, 0)).Rotator(), 0.05f);
		MeshComponent->SetBoneRotationByName("Gun", newRotation, EBoneSpaces::WorldSpace);
	}
}

void ARETank::ShootingTargetCannon()
{
	TArray<AActor*> TargetsInRange;
	GetOverlappingActors(TargetsInRange, AREZombieBase::StaticClass());

	currentCannonTarget = nullptr;
	if (TargetsInRange.Num() > 0)
	{
		AActor* tempCurrentTarget = nullptr;
		float range = 1;
		FHitResult Hit;
		FVector ShootDir;

		for (AActor* OtherTarget : TargetsInRange)
		{
			FVector TempShootDir = OtherTarget->GetActorLocation() - GetActorLocation();
			float tempRange = FVector::Dist(GetActorLocation(), OtherTarget->GetActorLocation());
			if (range < tempRange && HasLineOfSight(Hit, TempShootDir))
			{
				if (Hit.GetActor() == OtherTarget)
				{
					tempCurrentTarget = OtherTarget;
					range = tempRange;
					ShootDir = TempShootDir;
				}
			}
		}
		if (tempCurrentTarget != nullptr)
		{
			currentCannonTarget = Cast<AREZombieBase>(tempCurrentTarget);
			MulticastFireEffects(MeshComponent, FName("BarrelSocket"), CannonEffectScale, CannonMuzzleEffect, CannonShotSound);
			MulticastImpactEffects(MeshComponent, FName("BarrelSocket"), Hit.Location, CannonImpactEffectScale, CannonCharacterImpactEffect, CannonCharacterImpactSound);
			FPointDamageEvent DamageEvent(CannonDamage, Hit, ShootDir, nullptr);
			currentCannonTarget->TakeDamage(CannonDamage, DamageEvent, GetController(), this);
		}
	}
}

void ARETank::ShootingTargetGun()
{
	TArray<AActor*> TargetsInRange;
	GetOverlappingActors(TargetsInRange, AREZombieBase::StaticClass());

	currentTarget = nullptr;
	if (TargetsInRange.Num() > 0)
	{
		AActor* tempCurrentTarget = nullptr;
		float range = defenseRangeGun + 1;
		FHitResult Hit;
		FVector ShootDir;

		for (AActor* OtherTarget : TargetsInRange)
		{
			FVector TempShootDir = OtherTarget->GetActorLocation() - GetActorLocation();
			float tempRange = FVector::Dist(GetActorLocation(), OtherTarget->GetActorLocation());
			if (range > tempRange && HasLineOfSight(Hit, TempShootDir))
			{
				if (Hit.GetActor() == OtherTarget)
				{
					tempCurrentTarget = OtherTarget;
					range = tempRange;
					ShootDir = TempShootDir;
				}
			}
		}
		if (tempCurrentTarget != nullptr)
		{
			currentTarget = Cast<AREZombieBase>(tempCurrentTarget);
			MulticastFireEffects(MeshComponent, FName("GunSocket"), GunEffectScale, MuzzleEffect);
			MulticastImpactEffects(MeshComponent, FName("GunSocket"), Hit.Location, GunImpactEffectScale);
			FPointDamageEvent DamageEvent(GunDamage, Hit, ShootDir, nullptr);
			currentTarget->TakeDamage(GunDamage, DamageEvent, GetController(), this);
		}
	}
}

bool ARETank::HasLineOfSight(FHitResult& Hit, FVector ShootDir)
{
	FVector EyeLocation = GetActorLocation();
	FRotator EyeRotation = GetActorRotation();
	FVector End = EyeLocation + ShootDir * 10000;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());
	Params.bTraceComplex = true;
	Params.bReturnPhysicalMaterial = true;

	return GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, End, COLLISION_WEAPON, Params);
}

void ARETank::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARETank, currentCannonTarget);
}
