#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "REPlayerController.generated.h"

class AREInteractablePlaceBase;
class UInGameHUD;
class UREInteractablePointHUD;
class UREQuitGameMenu;
class UREGameOverMenu;

UCLASS()
class ROTTENEDEN_API AREPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SpawnInGameHUD();

	void PlayCrosshairAnimation() const;

	void OnPlayerDeath(AActor* ActorToFocus);

	void SetCrosshairVisibility(const bool& bIsVisible) const;

	bool GetCrosshairVisibility() const;

	void SetGunScopeViability(UTexture2D* GunScopeImage) const;

	void UpdateHealthUI(const float& Health, const float& MaxHealth, const float& HealDelta) const;

	void UpdateMedkitUI(bool&& HasConsumed) const;

	void UpdateAmmoUI(const int16& MagazineAmmo, const int16& TotalAmmo, const bool& PlayAnim, UTexture2D* WeaponImage) const;

	void UpdatePlayerCredits(const int16& Reward);

	void NotifyWaveState(const int32& WaveNumber, bool WaveHasEnded);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSpectating();
	
	UFUNCTION(Client, Reliable)
    void ClientOnObjectiveDestroyedPrompt(const int16& RemainingObjectives);

	UFUNCTION(Client, Reliable)
	void SetClientInteractablePointVisibility(bool bIsVisible,const FName& PromptMessage, const int8& MenuIndex, AREInteractablePlaceBase* InteractablePlace);

	UFUNCTION(Client, Reliable)
	void ClientNotifyWaveState(const int32& WaveNumber, bool WaveHasEnded);

	UFUNCTION(Client, Reliable)
	void ClientShowGameOverMenu();

	virtual void SetupInputComponent() override;

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UREInteractablePointHUD> InteractablePointHUDClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInGameHUD> InGameHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UREQuitGameMenu> QuitGameMenuClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UREGameOverMenu> GameOverMenuClass;
	
private:
	UPROPERTY()
	UInGameHUD* InGameHUD;

	UPROPERTY()
	UREInteractablePointHUD* InteractablePointHUD;

	UPROPERTY()
	UREQuitGameMenu* QuitGameMenu;

	UPROPERTY()
	UREGameOverMenu* GameOverMenu;

	UPROPERTY()
	TArray<UUserWidget*> UIElementsStack;

	UFUNCTION()
	void OnUIVisibilityChanged(UUserWidget* Broadcaster, const bool IsVisible);
	
	 void OpenInteractablePlaceMenu();

	void OpenMenu();

	UFUNCTION(Server, Reliable, WithValidation)
	void AddCreditsCheat();

	UFUNCTION(Client, Reliable)
	void ClientUpdateCreditsUI(const int16& CreditsDelta, const int16& TotalCredits);
};

