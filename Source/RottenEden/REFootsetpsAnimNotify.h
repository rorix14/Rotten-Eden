#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "REFootsetpsAnimNotify.generated.h"

class UNiagaraSystem;

UCLASS()
class ROTTENEDEN_API UREFootsetpsAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	UPROPERTY(EditAnywhere)
	bool bIsLanding;

	UPROPERTY(EditAnywhere)
	bool bIsRightFoot;

	UPROPERTY(EditAnywhere)
	bool bOnOwnerOnly;

	UPROPERTY(EditAnywhere)
	USoundBase* FootstepSoundGrass;

	UPROPERTY(EditAnywhere)
	USoundBase* FootstepSoundDirt;

	UPROPERTY(EditAnywhere)
	USoundBase* FootstepSoundConcrete;

	UPROPERTY(EditAnywhere)
	USoundBase* FootstepSoundMetal;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* FootstepEffectGrass;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* FootstepEffectDirt;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* FootstepEffectConcrete;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* FootstepEffectMetal;

	void PlayEffects(const UWorld* World, const APawn* PawnOwner, const FVector& OwnerLocation,
	                 USkeletalMeshComponent* MeshComp, USoundBase* FootstepSound, UNiagaraSystem* FootstepEffect);

	void PlayLandingEffects(const UWorld* World, USoundBase* FootstepSound, const FVector& OwnerLocation,
	                        UNiagaraSystem* FootstepEffect, USkeletalMeshComponent* MeshComp);
};
