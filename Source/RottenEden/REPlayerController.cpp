// Fill out your copyright notice in the Description page of Project Settings.


#include "REPlayerController.h"
#include "InGameHUD.h"
#include <Components/Overlay.h>
#include "REBuildingHUD.h"
#include "REInteractablePointHUD.h"
#include "REPlayerState.h"
#include "REQuitGameMenu.h"
#include "REGameOverMenu.h"

void AREPlayerController::SpawnInGameHUD()
{
	const FInputModeGameOnly InputModeGame;
	SetInputMode(InputModeGame);
	bShowMouseCursor = false;

	//Add UI to viewport based on order 
	if (!InGameHUD)
		InGameHUD = CreateWidget<UInGameHUD>(this, InGameHUDClass);

	if (InGameHUD)
	{
		InGameHUD->AddToPlayerScreen();
		SetCrosshairVisibility(true);
	}

	if (!InteractablePointHUD)
		InteractablePointHUD = CreateWidget<UREInteractablePointHUD>(this, InteractablePointHUDClass);

	if (InteractablePointHUD)
	{
		InteractablePointHUD->AddToPlayerScreen();
		InteractablePointHUD->SetVisibility(ESlateVisibility::Hidden);
		InteractablePointHUD->OnVisible.AddDynamic(this, &AREPlayerController::OnUIVisibilityChanged);
	}

	if (!QuitGameMenu)
		QuitGameMenu = CreateWidget<UREQuitGameMenu>(this, QuitGameMenuClass);

	if (QuitGameMenu)
	{
		if (QuitGameMenu->IsInViewport())
			QuitGameMenu->RemoveFromViewport();

		QuitGameMenu->AddToPlayerScreen();
		QuitGameMenu->SetVisibility(ESlateVisibility::Hidden);
		QuitGameMenu->OnVisible.AddDynamic(this, &AREPlayerController::OnUIVisibilityChanged);
	}
}

void AREPlayerController::OnPlayerDeath(AActor* ActorToFocus)
{
	if (InGameHUD)
		InGameHUD->RemoveFromViewport();

	if (InteractablePointHUD)
		InteractablePointHUD->RemoveFromViewport();

	//SetViewTargetWithBlend(ActorToFocus, 20.f);
}

void AREPlayerController::ServerStartSpectating_Implementation()
{
	PlayerState->SetIsSpectator(true);
	//bPlayerIsWaiting = true;
	ChangeState(NAME_Spectating);
	ClientGotoState(NAME_Spectating);

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const auto PC =It->Get();
		if (PC && PC != this &&PC->GetPawn())
		{
			ViewAPlayer(1);
			return;
		}
	}
}

bool AREPlayerController::ServerStartSpectating_Validate() { return true; }

void AREPlayerController::PlayCrosshairAnimation() const
{
	if (InGameHUD)
		InGameHUD->PlayAnimation(InGameHUD->GetCrosshairAnimation());
}

void AREPlayerController::SetCrosshairVisibility(const bool& bIsVisible) const
{
	if (InGameHUD)
		InGameHUD->GetCrosshairOverlay()->SetVisibility(
			bIsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

bool AREPlayerController::GetCrosshairVisibility() const
{
	if (InGameHUD)
		return InGameHUD->GetCrosshairOverlay()->GetVisibility() == ESlateVisibility::HitTestInvisible;

	return false;
}

void AREPlayerController::SetGunScopeViability(UTexture2D* GunScopeImage) const
{
	if (InGameHUD)
		InGameHUD->SetWeaponScopeImage(GunScopeImage,
		                               GunScopeImage ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

void AREPlayerController::UpdateHealthUI(const float& Health, const float& MaxHealth, const float& HealDelta) const
{
	if (!InGameHUD) return;

	InGameHUD->UpdateHealthUI(Health, MaxHealth, HealDelta);
	if (HealDelta != 0)
		InGameHUD->PlayAnimation(InGameHUD->GetHealthTextAnim(), 0.0f, 1, EUMGSequencePlayMode::Forward, 2.5f);
}

void AREPlayerController::UpdateMedkitUI(bool&& HasConsumed) const
{
	if (!InGameHUD) return;

	if (HasConsumed)
		InGameHUD->ConsumeMedkitUI();
	else
		InGameHUD->AddMedkitUI();
}

void AREPlayerController::UpdateAmmoUI(const int16& MagazineAmmo, const int16& TotalAmmo, const bool& PlayAnim,
                                       UTexture2D* WeaponImage) const
{
	if (!InGameHUD) return;

	InGameHUD->UpdateAmmoUI(MagazineAmmo, TotalAmmo);

	if (PlayAnim)
	{
		InGameHUD->SetWeaponImage(WeaponImage);
		InGameHUD->PlayAnimation(InGameHUD->GetAmmoInfoAnim(), 0.0f, 1, EUMGSequencePlayMode::Forward, 1.5f);
	}
}

void AREPlayerController::UpdatePlayerCredits(const int16& Reward)
{
	if (auto PS = GetPlayerState<AREPlayerState>())
	{
		PS->AddCreditsAndScore(Reward);
		ClientUpdateCreditsUI(Reward, PS->GetPlayerCredits());
	}
}

void AREPlayerController::ClientUpdateCreditsUI_Implementation(const int16& CreditsDelta, const int16& TotalCredits)
{
	if (InGameHUD)
		InGameHUD->UpdateCreditsUI(CreditsDelta, TotalCredits);
}

void AREPlayerController::NotifyWaveState(const int32& WaveNumber, bool WaveHasEnded)
{
	if (auto PS = GetPlayerState<AREPlayerState>())
	{
		if (WaveHasEnded)
			PS->AddScoreBonus(WaveNumber);

		ClientNotifyWaveState(WaveNumber, WaveHasEnded);
	}
}

void AREPlayerController::ClientNotifyWaveState_Implementation(const int32& WaveNumber, bool WaveHasEnded)
{
	if (InGameHUD)
		InGameHUD->WaveSateNotification(WaveNumber, WaveHasEnded);
}

void AREPlayerController::ClientOnObjectiveDestroyedPrompt_Implementation(const int16& RemainingObjectives)
{
	if (InGameHUD)
		InGameHUD->ShowObjectiveDestroyedNotification(RemainingObjectives);
}

void AREPlayerController::SetClientInteractablePointVisibility_Implementation(
	bool bIsVisible, const FName& PromptMessage, const int8& MenuIndex, AREInteractablePlaceBase* InteractablePlace)
{
	if (InteractablePointHUD)
	{
		InteractablePointHUD->SetInteractablePlaceBase(InteractablePlace);
		InteractablePointHUD->SetPlayerHud(InGameHUD);
		InteractablePointHUD->SetPrompText(FText::FromName(PromptMessage));
		InteractablePointHUD->SetMenuIndex(MenuIndex);
		InteractablePointHUD->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

// delegate that checks for UI elements visibility and manages player input accordingly
void AREPlayerController::OnUIVisibilityChanged(UUserWidget* Broadcaster, const bool IsVisible)
{
	if (!Broadcaster) return;

	if (IsVisible)
		UIElementsStack.Push(Broadcaster);
	else
	{
		if (UIElementsStack.Contains(Broadcaster))
			UIElementsStack.Pop();
	}

	APawn* ControlledPawn = GetPawn();
	if (UIElementsStack.Num() > 0)
	{
		FInputModeGameAndUI InputModeUI;
		InputModeUI.SetWidgetToFocus(Broadcaster->TakeWidget());
		InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputModeUI);
		bShowMouseCursor = true;
		if (ControlledPawn)
			ControlledPawn->DisableInput(this);
	}
	else
	{
		const FInputModeGameOnly InputModeGame;
		SetInputMode(InputModeGame);
		bShowMouseCursor = false;
		if (ControlledPawn)
			ControlledPawn->EnableInput(this);
	}
}

void AREPlayerController::ClientShowGameOverMenu_Implementation()
{
	DisableInput(this);

	if ((GameOverMenu = GameOverMenu ? GameOverMenu : CreateWidget<UREGameOverMenu>(this, GameOverMenuClass)) != nullptr
	)
	{
		OnUIVisibilityChanged(GameOverMenu, true);
		GameOverMenu->AddToViewport();
		GameOverMenu->SetUpGameOverMenu();
	}
}

void AREPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("Build"), IE_Pressed, this, &AREPlayerController::OpenInteractablePlaceMenu);
	InputComponent->BindAction(TEXT("OpenMenu"), IE_Pressed, this, &AREPlayerController::OpenMenu);
	InputComponent->BindAction(TEXT("AddCredistsCheat"), IE_Pressed, this, &AREPlayerController::AddCreditsCheat);
}

void AREPlayerController::PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel)
{
	Super::PreClientTravel(PendingURL, TravelType, bIsSeamlessTravel);

	OnPlayerDeath(nullptr);
	if (QuitGameMenu)
		QuitGameMenu->RemoveFromViewport();

	if (GameOverMenu)
		GameOverMenu->RemoveFromViewport();

	UIElementsStack.Empty();
}

void AREPlayerController::OpenInteractablePlaceMenu()
{
	if (InteractablePointHUD && InteractablePointHUD->GetVisibility() == ESlateVisibility::Visible &&
		UIElementsStack.Num() <= 0)
		InteractablePointHUD->SwitchMenu();
}

void AREPlayerController::OpenMenu()
{
	if (QuitGameMenu)
		QuitGameMenu->InteractWithQuitGameMenu();
}

void AREPlayerController::AddCreditsCheat_Implementation()
{
	UpdatePlayerCredits(100);
}

bool AREPlayerController::AddCreditsCheat_Validate() { return true; }
