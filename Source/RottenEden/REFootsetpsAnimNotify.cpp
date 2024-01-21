#include "REFootsetpsAnimNotify.h"
#include <GameFramework/Character.h>
#include <Kismet/GameplayStatics.h>
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "RottenEden.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

void UREFootsetpsAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		const UWorld* World = OwnerActor->GetWorld();
		if (!World) return;

		const FVector OwnerLocation = OwnerActor->GetActorLocation();

		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(OwnerActor);
		QueryParams.bReturnPhysicalMaterial = true;

		if (World->LineTraceSingleByChannel(Hit, OwnerLocation, (OwnerLocation - FVector(0, 0, 300)), ECC_Visibility,
		                                    QueryParams))
		{
			if (auto PawnOwner = Cast<APawn>(OwnerActor))
			{
				PawnOwner->MakeNoise(0.2f, PawnOwner, PawnOwner->GetActorLocation());
				
				switch (Hit.PhysMaterial->SurfaceType)
				{
				case SURFACE_CONCRETE:
					PlayEffects(World, PawnOwner, OwnerLocation, MeshComp, FootstepSoundConcrete,
					            FootstepEffectConcrete);
					break;
				case SURFACE_DIRT:
					PlayEffects(World, PawnOwner, OwnerLocation, MeshComp, FootstepSoundDirt,
					            FootstepEffectDirt);
					break;
				case SURFACE_GRASS:
					PlayEffects(World, PawnOwner, OwnerLocation, MeshComp, FootstepSoundGrass,
					            FootstepEffectGrass);
					break;
				case SURFACE_METAL:
					PlayEffects(World, PawnOwner, OwnerLocation, MeshComp, FootstepSoundMetal,
                                FootstepEffectMetal);
					break;
				default:
					PlayEffects(World, PawnOwner, OwnerLocation, MeshComp, FootstepSoundConcrete,
					            FootstepEffectConcrete);
					break;
				}
			}
		}
	}
}

void UREFootsetpsAnimNotify::PlayEffects(const UWorld* World, const APawn* PawnOwner, const FVector& OwnerLocation,
                                         USkeletalMeshComponent* MeshComp, USoundBase* FootstepSound,
                                         UNiagaraSystem* FootstepEffect)
{
	if (!bIsLanding)
	{
		const FVector Foot = bIsRightFoot
			                     ? MeshComp->GetSocketLocation(TEXT("Foot_RSocket"))
			                     : MeshComp->GetSocketLocation(TEXT("Foot_LSocket"));

		UGameplayStatics::SpawnSoundAtLocation(World, FootstepSound, PawnOwner->GetActorLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, FootstepEffect, Foot);
	}
	else
	{
		const bool IsLocallyControlled = PawnOwner->IsLocallyControlled();
		if (bOnOwnerOnly && IsLocallyControlled)
			PlayLandingEffects(World, FootstepSound, OwnerLocation, FootstepEffect, MeshComp);

		if (!bOnOwnerOnly && !IsLocallyControlled)
			PlayLandingEffects(World, FootstepSound, OwnerLocation, FootstepEffect, MeshComp);
	}
}

void UREFootsetpsAnimNotify::PlayLandingEffects(const UWorld* World, USoundBase* FootstepSound,
                                                const FVector& OwnerLocation, UNiagaraSystem* FootstepEffect,
                                                USkeletalMeshComponent* MeshComp)
{
	UGameplayStatics::SpawnSoundAtLocation(World, FootstepSound, OwnerLocation);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, FootstepEffect, MeshComp->GetSocketLocation(TEXT("Foot_RSocket")));
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, FootstepEffect, MeshComp->GetSocketLocation(TEXT("Foot_LSocket")));
}
