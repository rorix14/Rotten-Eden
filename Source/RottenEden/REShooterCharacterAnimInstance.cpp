#include "REShooterCharacterAnimInstance.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "REShooterCharacter.h"


UREShooterCharacterAnimInstance::UREShooterCharacterAnimInstance()
{
	Speed = 0.0f;
	Angle = 0.0f;
	AimPitch = 0.0f;
	bIsAirborne = false;
	bIsReloading = false;
}

void UREShooterCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (AActor* OwningActor = GetOwningActor())
	{
		const FVector ActorVelocity = OwningActor->GetVelocity();
		Speed = ActorVelocity.Size();
		Angle = OwningActor->GetActorTransform().InverseTransformVectorNoScale(ActorVelocity).Rotation().Yaw;

		if (AREShooterCharacter* OwningCharacter = Cast<AREShooterCharacter>(OwningActor))
		{
			bIsAirborne = OwningCharacter->GetMovementComponent()->IsFalling();
			bIsDead = OwningCharacter->GetDied();
			bIsReloading = OwningCharacter->CheckForState(EPlayerAnimState::IsReloading);
			bIsSwitchingWeapon = OwningCharacter->CheckForState(EPlayerAnimState::IsChangingWeapon);
			bIsSprinting = OwningCharacter->GetWantsToSprint();

			const float RotationAimPitch = OwningCharacter->GetBaseAimRotation().Pitch;
			AimPitch = RotationAimPitch >= 180 ? RotationAimPitch - 360 : RotationAimPitch;
		}
	}
}
