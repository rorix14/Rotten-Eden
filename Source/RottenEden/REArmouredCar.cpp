// Fill out your copyright notice in the Description page of Project Settings.


#include "REArmouredCar.h"
#include "REZombieBase.h"
#include "RottenEden.h"
#include "Components/SphereComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"

AREArmouredCar::AREArmouredCar()
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

void AREArmouredCar::BeginPlay()
{
	Super::BeginPlay();

	AudioLoopComp->Play();

	if (HasAuthority())
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetewenShots, this, &AREArmouredCar::ShootingTarget, shootingInterval, true);
}

void AREArmouredCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (currentTarget != nullptr)
	{
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = currentTarget->GetActorLocation();
		FVector Dir = (TargetLoc - MyLoc);
		Dir.Normalize();
		FRotator gunRotation = MeshComponent->GetBoneRotationByName("SA_Veh_BigUte_Turret", EBoneSpaces::WorldSpace);
		FRotator newRotation = FMath::Lerp(gunRotation, FRotationMatrix::MakeFromX(FVector(Dir.X, Dir.Y, 0)).Rotator(), 0.05f);
		MeshComponent->SetBoneRotationByName("SA_Veh_BigUte_Turret", newRotation, EBoneSpaces::WorldSpace);
	}
}

void AREArmouredCar::ShootingTarget()
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
			MulticastFireEffects(MeshComponent, FName("SA_Veh_BigUte_TurretSocket"), EffectScale);
			MulticastImpactEffects(MeshComponent, FName("SA_Veh_BigUte_TurretSocket"), Hit.Location, ImpactEffectScale);
			FPointDamageEvent DamageEvent(damage, Hit, ShootDir, nullptr);
			currentTarget->TakeDamage(damage, DamageEvent, GetController(), this);
		}
	}
}

bool AREArmouredCar::HasLineOfSight(FHitResult& Hit, FVector ShootDir)
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