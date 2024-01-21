#include "REShooterCharacter.h"
#include <DrawDebugHelpers.h>
#include <TimerManager.h>
#include <Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Net/UnrealNetwork.h>
#include "REFPGun.h"
#include "REGun.h"
#include "REHealthComponent.h"
#include "REInventoryComponent.h"
#include "REPlayerController.h"
#include "REPlayerMovementComponent.h"
#include "RottenEden.h"
#include "RottenEdenGameModeBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/SpringArmComponent.h"


AREShooterCharacter::AREShooterCharacter(const class FObjectInitializer& ObjectInitializer): Super(
	/* Override the movement class from the base class to our own class to support multiple speeds */
	ObjectInitializer.SetDefaultSubobjectClass<UREPlayerMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->SetupAttachment(GetRootComponent());

	if ((CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"))) != nullptr)
	{
		//CameraComp->bUsePawnControlRotation = true;
		CameraComp->SetupAttachment(CameraBoomComp);

		if ((FPMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"))) != nullptr)
		{
			FPMeshComp->SetupAttachment(CameraComp);
			FPMeshComp->bOnlyOwnerSee = true;
			FPMeshComp->CastShadow = false;
		}
	}

	if ((HealthComp = CreateDefaultSubobject<UREHealthComponent>(TEXT("HealthComp"))) != nullptr)
		HealthComp->OnHealthChanged.AddDynamic(this, &AREShooterCharacter::OnHealthChanged);

	InventoryComp = CreateDefaultSubobject<UREInventoryComponent>(TEXT("InventoryComp"));

	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitterComp"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	GetMesh()->bCastHiddenShadow = true;
	GetMesh()->bOwnerNoSee = true;

	CurrentAnimState = EPlayerAnimState::Default;
	ZoomIterSpeed = 20;
}

void AREShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	if (FPMeshComp)
	{
		DefaultWeaponPosition = FPMeshComp->GetRelativeLocation();
		DefaultWeaponRotation = FPMeshComp->GetRelativeRotation();
	}

	if (HasAuthority() && InventoryComp)
		CurrentWeapon = InventoryComp->GenerateWeaponInventory(GetMesh(), FPMeshComp);
}

void AREShooterCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if ((PlayerController = GetController<AREPlayerController>()) != nullptr)
	{
		PlayerController->SpawnInGameHUD();
		if (HealthComp)
			PlayerController->UpdateHealthUI(HealthComp->GetHealth(), HealthComp->GetMaxHealth(), 0);
	}
}

void AREShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentWeapon && !CurrentWeapon->HasVisibleWeapon())
		CurrentWeapon->SetWeaponMeshVisibility(true);

	if (CameraComp && CurrentWeapon)
	{
		if (GetWantsToSprint())
			AnimateFPWeapon(DeltaTime,
			                {
				                OscillateBetweenValues(-39.6f, -20.0f, 10.0f),
				                OscillateBetweenValues(-155.0f, -126.0f, 10.0f),
				                OscillateBetweenValues(21.6f, 17.0f, 10.0f)
			                }, {90.0f, DefaultWeaponPosition.Y, -48.0f}, DefaultFOV, 5.0f);
		else if (CheckForState(EPlayerAnimState::WantsToZoom))
			AnimateFPWeapon(DeltaTime, CurrentWeapon->GetFPGun()->GetZoomedRotation(),
			                CurrentWeapon->GetFPGun()->GetZoomedPosition(), CurrentWeapon->GetFPGun()->GetZoomedFOV(),
			                ZoomIterSpeed);
		else if (CheckForState(EPlayerAnimState::IsChangingWeapon))
			AnimateFPWeapon(DeltaTime, {DefaultWeaponRotation.Pitch, DefaultWeaponRotation.Yaw, -90},
			                {29, DefaultWeaponPosition.Y, 15}, DefaultFOV, 5.0f);
		else if (CheckForState(EPlayerAnimState::IsReloading))
			AnimateFPWeapon(DeltaTime, {DefaultWeaponRotation.Pitch, DefaultWeaponRotation.Yaw, 90},
			                DefaultWeaponPosition, DefaultFOV, 5.0f);
		else if (CheckForState(EPlayerAnimState::IsApplyingBandage))
			AnimateFPWeapon(DeltaTime, {DefaultWeaponRotation.Pitch, -180, 36}, {DefaultWeaponPosition.X, 43, -49},
			                DefaultFOV, 5.0f);
		else
			AnimateFPWeapon(DeltaTime, DefaultWeaponRotation, DefaultWeaponPosition, DefaultFOV, ZoomIterSpeed);
	}

	// DEBUGGING: TO SEE WHAT IS THE LOCAL AND REMOTE ROLE OF EACH PLAYER.
	// DrawDebugString(GetWorld(), FVector(0, 300, 100), "Local: " + UEnum::GetValueAsString(GetLocalRole()), this,
	//                 FColor::Red, 0);
	// DrawDebugString(GetWorld(), FVector(0, 300, 120), "Remote: " + UEnum::GetValueAsString(GetRemoteRole()), this,
	//                 FColor::Yellow, 0);
}

void AREShooterCharacter::AnimateFPWeapon(const float DeltaTime, const FRotator& TargetRotation,
                                          const FVector& TargetLocation, const float& TargetFOV,
                                          const float& IterSpeed) const
{
	FPMeshComp->SetRelativeTransform(FTransform(
		FQuat(FMath::Lerp(FPMeshComp->GetRelativeRotation(), TargetRotation, IterSpeed * DeltaTime)),
		FMath::Lerp(FPMeshComp->GetRelativeLocation(), TargetLocation, IterSpeed * DeltaTime),
		FPMeshComp->GetRelativeScale3D()));

	CameraComp->SetFieldOfView(FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomIterSpeed));
}

float AREShooterCharacter::OscillateBetweenValues(const float& Start, const float& End, const float& Frequency) const
{
	const float OscillationRange = (Start - End) / 2;
	const float OscillationOffset = OscillationRange + End;
	return OscillationOffset + FMath::Sin(GetWorld()->TimeSeconds * Frequency) * OscillationRange;
}

bool AREShooterCharacter::GetWantsToSprint() const
{
	return CheckForState(EPlayerAnimState::WantsToSprint) && !GetVelocity().IsZero() && FVector::DotProduct(
		GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8;
}

void AREShooterCharacter::SwitchGun(float Value)
{
	if (CurrentWeapon)
		CurrentWeapon->SetWeaponMeshVisibility(false);

	if (InventoryComp)
		CurrentWeapon = InventoryComp->InputChangeGun(static_cast<int16>(Value));

	CurrentAnimState = EPlayerAnimState::Default;
}

void AREShooterCharacter::SetWeaponUIElementsVisibility(bool&& CrosshairVisibility, bool&& WeaponScopeVisibility) const
{
	if (!PlayerController) return;

	if (!(CrosshairVisibility && PlayerController->GetCrosshairVisibility()))
		PlayerController->SetCrosshairVisibility(CrosshairVisibility);

	if (CurrentWeapon && CurrentWeapon->GetFPGun())
		PlayerController->SetGunScopeViability(
			CurrentWeapon->GetFPGun()->DoGunAimEffects(WeaponScopeVisibility));
}

float AREShooterCharacter::GetMouseSensitivity() const
{
	switch (CurrentAnimState)
	{
	case EPlayerAnimState::WantsToSprint:
		return MouseSensitivity / 2.5f;
	case EPlayerAnimState::WantsToZoom:
		return MouseSensitivity / 2;
	default:
		return MouseSensitivity;
	}
}

void AREShooterCharacter::OnHealthChanged(UREHealthComponent* OwningHealthComp, const float& Health,
                                          const float& HealthDelta, const UDamageType* DamageType,
                                          AController* InstigatedBy, AActor* DamageCauser)
{
	if (OwningHealthComp && PlayerController && IsLocallyControlled())
		PlayerController->UpdateHealthUI(Health, OwningHealthComp->GetMaxHealth(), HealthDelta);

	OnStopSprinting();

	//UE_LOG(LogTemp, Warning, TEXT("Role %s , health: %s"), *UEnum::GetValueAsString(GetLocalRole()), *FString::SanitizeFloat(Health));
	if (Health <= 0.0f)
	{
		if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
		{
			CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
		}

		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
			MeshComp->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Block);
		}

		if (UCharacterMovementComponent* CharacterComp = GetCharacterMovement())
		{
			CharacterComp->StopMovementImmediately();
			CharacterComp->DisableMovement();
			CharacterComp->SetComponentTickEnabled(false);
		}

		EndShoot();

		if (PlayerController && PlayerController->IsLocalController())
		{
			PlayerController->ServerStartSpectating();
			PlayerController->OnPlayerDeath(this);
		}

		if (HasAuthority())
		{
			if (const auto GameMode = GetWorld()->GetAuthGameMode<ARottenEdenGameModeBase>())
				GameMode->OnPlayerDied();
			
			bDied = true;
			//TearOff();
			DetachFromControllerPendingDestroy();
			SetLifeSpan(15.0f);
		}
	}
}

void AREShooterCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (InventoryComp && HasAuthority() && EndPlayReason == EEndPlayReason::Destroyed)
		InventoryComp->ClearInventory();
}

void AREShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AREShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AREShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AREShooterCharacter::MouseLookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AREShooterCharacter::MouseLookRight);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AREShooterCharacter::GamePadLookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AREShooterCharacter::GamePadLookRight);

	PlayerInputComponent->BindAction("SprintHold", IE_Pressed, this, &AREShooterCharacter::OnStartSprinting);
	PlayerInputComponent->BindAction("SprintHold", IE_Released, this, &AREShooterCharacter::OnStopSprinting);

	PlayerInputComponent->BindAxis(TEXT("Change Weapon"), this, &AREShooterCharacter::ChangeWeapon);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Pressed, this, &AREShooterCharacter::BeginShoot);
	PlayerInputComponent->BindAction(TEXT("Shoot"), IE_Released, this, &AREShooterCharacter::EndShoot);

	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Pressed, this, &AREShooterCharacter::BeginZoom);
	PlayerInputComponent->BindAction(TEXT("Zoom"), IE_Released, this, &AREShooterCharacter::EndZoom);

	PlayerInputComponent->BindAction(TEXT("ReloadWeapon"), IE_Pressed, this, &AREShooterCharacter::ReloadWeapon);

	PlayerInputComponent->BindAction(TEXT("UsedMedPack"), IE_Pressed, this, &AREShooterCharacter::UseHealthPack);
}

void AREShooterCharacter::MoveForward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AREShooterCharacter::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AREShooterCharacter::MouseLookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue * GetMouseSensitivity());
}

void AREShooterCharacter::MouseLookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue * GetMouseSensitivity());
}

void AREShooterCharacter::GamePadLookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AREShooterCharacter::GamePadLookRight(float AxisValue)
{
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AREShooterCharacter::OnStartSprinting()
{
	if (CheckForState(EPlayerAnimState::IsChangingWeapon) || CheckForState(EPlayerAnimState::IsReloading) ||
		CheckForState(EPlayerAnimState::IsApplyingBandage))
		return;

	EndShoot();
	SetWeaponUIElementsVisibility(false);
	ServerSetUpSprint(true);
}

void AREShooterCharacter::OnStopSprinting()
{
	if (CheckForState(EPlayerAnimState::WantsToSprint))
	{
		SetWeaponUIElementsVisibility(true);
		ServerSetUpSprint(false);
	}
}

void AREShooterCharacter::UseHealthPack()
{
	if (InventoryComp && !InventoryComp->CanConsumeHealthPack() || CheckForState(EPlayerAnimState::IsChangingWeapon) ||
		CheckForState(EPlayerAnimState::IsReloading) || CheckForState(EPlayerAnimState::IsApplyingBandage))
		return;

	EndShoot();
	SetWeaponUIElementsVisibility(true);
	ServerUseHealthPack();
}

void AREShooterCharacter::ChangeWeapon(float AxisValue)
{
	if (!CurrentWeapon || static_cast<int>(AxisValue) == 0 || CheckForState(EPlayerAnimState::IsChangingWeapon) ||
		CheckForState(EPlayerAnimState::IsReloading) || CheckForState(EPlayerAnimState::IsApplyingBandage))
		return;

	CurrentWeapon->UnequipGun();
	SetWeaponUIElementsVisibility(true);
	ServerSwitchGun(AxisValue);
}

void AREShooterCharacter::BeginShoot()
{
	if (!CurrentWeapon || CheckForState(EPlayerAnimState::IsChangingWeapon) ||
		CheckForState(EPlayerAnimState::IsReloading) || CheckForState(EPlayerAnimState::IsApplyingBandage))
		return;

	OnStopSprinting();
	CurrentWeapon->StartFire();
}

void AREShooterCharacter::EndShoot()
{
	if (CurrentWeapon)
		CurrentWeapon->EndFire();
}

void AREShooterCharacter::BeginZoom()
{
	if (CheckForState(EPlayerAnimState::IsChangingWeapon) ||
		CheckForState(EPlayerAnimState::IsReloading) || CheckForState(EPlayerAnimState::IsApplyingBandage))
		return;

	SetWeaponUIElementsVisibility(false, true);
	ServerWantsToZoom(true);
}

void AREShooterCharacter::EndZoom()
{
	if (CheckForState(EPlayerAnimState::WantsToZoom))
	{
		SetWeaponUIElementsVisibility(true);
		ServerWantsToZoom(false);
	}
}

void AREShooterCharacter::ReloadWeapon()
{
	if (!CurrentWeapon || CheckForState(EPlayerAnimState::IsChangingWeapon) ||
		CheckForState(EPlayerAnimState::IsReloading) || CheckForState(EPlayerAnimState::IsApplyingBandage))
		return;

	SetWeaponUIElementsVisibility(true);
	EndShoot();
	ServerReload();
}

void AREShooterCharacter::ServerReload_Implementation()
{
	CurrentAnimState = CurrentWeapon->ReloadWeapon() ? EPlayerAnimState::IsReloading : EPlayerAnimState::Default;
	GetWorldTimerManager().SetTimer(TimerHandle_ReloadingTime, this, &AREShooterCharacter::EndReload, 1.4f, false);
}

bool AREShooterCharacter::ServerReload_Validate() { return true; }

void AREShooterCharacter::OnRep_GunSwitched(AREGun* OldWeapon)
{
	if (OldWeapon)
		OldWeapon->SetWeaponMeshVisibility(false);
}

void AREShooterCharacter::ServerSwitchGun_Implementation(float Value)
{
	if (InventoryComp && !InventoryComp->CanChangeGun()) return;

	CurrentAnimState = EPlayerAnimState::IsChangingWeapon;
	GetWorldTimerManager().SetTimer(TimerHandle_SwitchGun,
	                                FTimerDelegate::CreateUObject(this, &AREShooterCharacter::SwitchGun, Value), 1.0f,
	                                false);
}

bool AREShooterCharacter::ServerSwitchGun_Validate(float Value) { return true; }

void AREShooterCharacter::ServerUseHealthPack_Implementation()
{
	if (!HealthComp || !InventoryComp) return;

	/*Change animation sate based if it is possible to heal or not, so crosshair visibility works as intended*/
	if (HealthComp->Heal(40.0f))
	{
		InventoryComp->AddToHealthPack(-1);
		CurrentAnimState = EPlayerAnimState::IsApplyingBandage;
		GetWorldTimerManager().SetTimer(TimerHandle_ApplyingBandageTime, this, &AREShooterCharacter::EndApplyBandage,
		                                1.4f, false);
	}
}

bool AREShooterCharacter::ServerUseHealthPack_Validate() { return true; }

void AREShooterCharacter::ServerWantsToZoom_Implementation(bool WantsToZoom)
{
	CurrentAnimState = WantsToZoom ? EPlayerAnimState::WantsToZoom : EPlayerAnimState::Default;
}

bool AREShooterCharacter::ServerWantsToZoom_Validate(bool WantsToZoom) { return true; }

void AREShooterCharacter::ServerSetUpSprint_Implementation(bool WantsToSprint)
{
	CurrentAnimState = WantsToSprint ? EPlayerAnimState::WantsToSprint : EPlayerAnimState::Default;
}

bool AREShooterCharacter::ServerSetUpSprint_Validate(bool WantsToSprint) { return true; }

void AREShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AREShooterCharacter, bDied);
	DOREPLIFETIME(AREShooterCharacter, CurrentAnimState);
	DOREPLIFETIME(AREShooterCharacter, CurrentWeapon);
}
