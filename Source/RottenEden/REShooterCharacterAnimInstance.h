#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "REShooterCharacterAnimInstance.generated.h"

UCLASS()
class ROTTENEDEN_API UREShooterCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UREShooterCharacterAnimInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	float Angle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	float AimPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsAirborne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsReloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsSwitchingWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generic")
	bool bIsSprinting;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
