// Fill out your copyright notice in the Description page of Project Settings.


#include "REArmedGuard.h"
#include "REZombieBase.h"
#include "RottenEden.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

AREArmedGuard::AREArmedGuard()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RangeSphere = CreateDefaultSubobject<USphereComponent>("Range Sphere");
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh Component");
	RangeSphere->SetupAttachment(RootComponent);
	MeshComponent->SetupAttachment(RootComponent);
}

void AREArmedGuard::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		DefenceWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		DefenceWeapon->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Hand_RSocket"));
		WeaponMesh = Cast<USkeletalMeshComponent>(DefenceWeapon->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetewenShots, this, &AREArmedGuard::ShootingTarget, shootingInterval, true);
	}
}

void AREArmedGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsDead) return;

	if (currentTarget != nullptr)
	{
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = currentTarget->GetActorLocation();
		FVector Dir = (TargetLoc - MyLoc);
		Dir.Normalize();
		SetActorRotation(FMath::Lerp(GetActorRotation(),
			FRotationMatrix::MakeFromX(FVector(Dir.X, Dir.Y, 0)).Rotator(),
			0.05f));
	}
}

void AREArmedGuard::ShootingTarget()
{
	if(IsDead) return;
	
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

bool AREArmedGuard::HasLineOfSight(FHitResult& Hit, FVector ShootDir)
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

void AREArmedGuard::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority() && DefenceWeapon && EndPlayReason == EEndPlayReason::Destroyed)
		DefenceWeapon->Destroy();
}

void AREArmedGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AREArmedGuard, DefenceWeapon, COND_SimulatedOnly);
}
