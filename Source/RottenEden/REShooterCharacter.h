#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "REShooterCharacter.generated.h"

class AREGun;
class UREHealthComponent;

UENUM()
enum class EPlayerAnimState : uint8
{
	WantsToSprint,
	WantsToZoom,
	IsChangingWeapon,
	IsReloading,
	IsApplyingBandage,
	Default
};

UCLASS()
class ROTTENEDEN_API AREShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AREShooterCharacter(const class FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool CheckForState(EPlayerAnimState&& StateToCheck) const { return CurrentAnimState == StateToCheck; }

	bool GetDied() const { return bDied; }
	bool GetWantsToSprint() const;

	virtual void PawnClientRestart() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* FPMeshComp;

private:
	UPROPERTY(Replicated)
	EPlayerAnimState CurrentAnimState;

	UPROPERTY()
	class AREPlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USpringArmComponent* CameraBoomComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UREHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UREInventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnNoiseEmitterComponent* NoiseEmitterComp;

	/* Used for controller input */
	UPROPERTY(EditAnywhere, Category = "Player")
	float RotationRate = 70.0f;
	
	UPROPERTY(EditAnywhere, Category = "Player")
	float MouseSensitivity = 1.0f;
	
	UPROPERTY(ReplicatedUsing = OnRep_GunSwitched)
	AREGun* CurrentWeapon;

	UPROPERTY(Replicated)
	bool bDied;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomIterSpeed;

	float DefaultFOV;

	FVector DefaultWeaponPosition;

	FRotator DefaultWeaponRotation;

	FTimerHandle TimerHandle_SwitchGun;

	FTimerHandle TimerHandle_ReloadingTime;

	FTimerHandle TimerHandle_ApplyingBandageTime;

	UFUNCTION()
	void OnRep_GunSwitched(AREGun* OldWeapon);

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MouseLookUp(float AxisValue);
	void MouseLookRight(float AxisValue);
	void GamePadLookUp(float AxisValue);
	void GamePadLookRight(float AxisValue);
	void ChangeWeapon(float AxisValue);
	void OnStartSprinting();
	void OnStopSprinting();
	void BeginShoot();
	void EndShoot();
	void BeginZoom();
	void EndZoom();
	void ReloadWeapon();
	void UseHealthPack();

	void AnimateFPWeapon(const float DeltaTime, const FRotator& TargetRotation, const FVector& TargetLocation,
	                     const float& TargetFOV, const float& IterSpeed) const;

	float OscillateBetweenValues(const float& Start, const float& End, const float& Frequency) const;

	void SwitchGun(float Value);

	void SetWeaponUIElementsVisibility(bool&& CrosshairVisibility, bool&& WeaponScopeVisibility = false) const;

	float GetMouseSensitivity() const;

	void EndReload() { CurrentAnimState = EPlayerAnimState::Default; }

	void EndApplyBandage() { CurrentAnimState = EPlayerAnimState::Default; }

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerWantsToZoom(bool WantsToZoom);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerReload();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwitchGun(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUseHealthPack();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetUpSprint(bool WantsToSprint);

	UFUNCTION()
	void OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health, const float& HealthDelta,
	                     const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
