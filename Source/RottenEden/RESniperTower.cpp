// Fill out your copyright notice in the Description page of Project Settings.


#include "RESniperTower.h"
#include "Components/SkeletalMeshComponent.h"
#include "REZombieBase.h"
#include "RottenEden.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ARESniperTower::ARESniperTower()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RangeSphere = CreateDefaultSubobject<USphereComponent>("Range Sphere");
	TowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Tower Mesh Component");
	SniperMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Sniper Mesh Component");
	RangeSphere->SetupAttachment(RootComponent);
	TowerMeshComponent->SetupAttachment(RootComponent);
	SniperMeshComponent->SetupAttachment(RootComponent);
}

void ARESniperTower::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		DefenceWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator,
		                                               SpawnParams);
		DefenceWeapon->AttachToComponent(SniperMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		                                 TEXT("Hand_RSocket"));
		WeaponMesh = Cast<USkeletalMeshComponent>(
			DefenceWeapon->GetComponentByClass(USkeletalMeshComponent::StaticClass()));

		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetewenShots, this, &ARESniperTower::ShootingTarget,
		                                shootingInterval, true);
	}
}

void ARESniperTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (currentTarget != nullptr)
	{
		// FVector MyLoc = GetActorLocation();
		// FVector TargetLoc = currentTarget->GetActorLocation();
		// FVector Dir = (TargetLoc - MyLoc);
		// Dir.Normalize();
		// FRotator SniperRotation = SniperMeshComponent->GetRelativeRotation() + GetActorRotation();
		// SniperMeshComponent->SetRelativeRotation(FMath::Lerp(SniperRotation,
		// 	FRotationMatrix::MakeFromX(FVector(Dir.X, Dir.Y, 0)).Rotator(),
		// 	0.05f));


		const FRotator LookDirection = FRotationMatrix::MakeFromX(
			currentTarget->GetActorLocation() - GetActorLocation()).Rotator();
		SniperMeshComponent->SetWorldRotation({0, LookDirection.Yaw - 90, 0});
	}
}

void ARESniperTower::ShootingTarget()
{
	TArray<AActor*> TargetsInRange;
	GetOverlappingActors(TargetsInRange, AREZombieBase::StaticClass());

	currentTarget = nullptr;
	if (TargetsInRange.Num() > 0)
	{
		AActor* tempCurrentTarget = nullptr;
		float range = defenseRange + 1;
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
			MulticastFireEffects(WeaponMesh, FName("MuzzleSocket"), EffectScale);
			MulticastImpactEffects(WeaponMesh, FName("MuzzleSocket"), Hit.Location, ImpactEffectScale);
			FPointDamageEvent DamageEvent(damage, Hit, ShootDir, nullptr);
			currentTarget->TakeDamage(damage, DamageEvent, GetController(), this);
		}
	}
}

bool ARESniperTower::HasLineOfSight(FHitResult& Hit, FVector ShootDir)
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


void ARESniperTower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority() && DefenceWeapon && EndPlayReason == EEndPlayReason::Destroyed)
		DefenceWeapon->Destroy();
}

void ARESniperTower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARESniperTower, DefenceWeapon, COND_SimulatedOnly);
}
