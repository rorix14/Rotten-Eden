// Fill out your copyright notice in the Description page of Project Settings.
#include "RETracerRound.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ARETracerRound::ARETracerRound()
{
	if ((StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"))) != nullptr)
	{
		RootComponent = StaticMeshComp;
		StaticMeshComp->CastShadow = false;
	}

	if ((ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"))) != nullptr)
	{
		ProjectileMovement->UpdatedComponent = RootComponent;
		ProjectileMovement->InitialSpeed = 10000.f;
		ProjectileMovement->MaxSpeed = 10000.f;
		ProjectileMovement->ProjectileGravityScale = 0;
		ProjectileMovement->bRotationFollowsVelocity = true;
		ProjectileMovement->bShouldBounce = false;
	}

	InitialLifeSpan = 10.0f;
}

void ARETracerRound::BeginPlay()
{
	Super::BeginPlay();

	if (StaticMeshComp)
		StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ARETracerRound::OnHit);

	if (ProjectileTracerEffect)
		UGameplayStatics::SpawnEmitterAttached(ProjectileTracerEffect, StaticMeshComp, NAME_None, FVector(ForceInit),
		                                       FRotator::ZeroRotator, FVector(1.5f));

	//SetActorLocation({GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z - 3});
}

void ARETracerRound::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (Cast<ARETracerRound>(OtherActor)) return;

	Destroy();
}
