#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUD.generated.h"

class UImage;
class UOverlay;
class UTextBlock;
class UProgressBar;
class UHorizontalBox;

UCLASS()
class ROTTENEDEN_API UInGameHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealthUI(const float& Health, const float& MaxHealth, const float& DamageTaken);

	void UpdateAmmoUI(const int16& MagazineAmmo, const int16& TotalAmmo) const;

	void UpdateCreditsUI(const int16& CreditsDelta, const int16& TotalCredits);

	UOverlay* GetCrosshairOverlay() const { return CrosshairOverlay; }
	UWidgetAnimation* GetCrosshairAnimation() const { return CrosshairAnimation; }
	UWidgetAnimation* GetHealthTextAnim() const { return HealthTextAnim; }
	UWidgetAnimation* GetAmmoInfoAnim() const { return AmmoInfoAnim; }
	UTextBlock* GetTotalCreditsText() const { return TotalCreditsText; }

	void SetWeaponImage(UTexture2D* Image) const;
	void SetWeaponScopeImage(UTexture2D* Image, ESlateVisibility&& ScopeVisibility) const;

	void ConsumeMedkitUI();

	void AddMedkitUI();

	void ShowObjectiveDestroyedNotification(const int16& RemainingObjectives);
	// might expand this to take a message string to accommodate for game over state.
	void WaveSateNotification(const int32& WaveNumber, const bool& WaveHasEnded);

protected:
	UPROPERTY(EditInstanceOnly)
	uint16 NumberOfMedkits;

	UPROPERTY(EditInstanceOnly)
	TSubclassOf<class UREMedkitUI> MedkitUI;

	virtual bool Initialize() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual void NativeConstruct() override;

	virtual void BeginDestroy() override;

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* OnWaveCompletedSound;

	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* OnStartedSound;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	USoundBase* OnDestroyedObjectiveNotificationSound;
	
	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* CrosshairAnimation;

	UPROPERTY(meta = (BindWidget))
	UOverlay* CrosshairOverlay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthTextAnimValue;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* HealthTextAnim;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UImage* HealthIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* GunImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoInMagazineText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* AmmoInfoAnim;

	UPROPERTY(meta = (BindWidget))
	UImage* DamageVignette;

	UPROPERTY(meta = (BindWidget))
	UImage* WeaponScope;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* MedkitContainer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalCreditsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeltaCreditsText;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* CreditsTextAnim;

	UPROPERTY(meta = (BindWidget))
	UTextBlock*	RemainingText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RemainingTentsText;

	UPROPERTY(meta = (BindWidgetAnim))
	UWidgetAnimation* TentDestroyedPromptAnim;
	
	UPROPERTY(meta = (BindWidget))
    UTextBlock* WaveStateText;

	UPROPERTY(meta = (BindWidgetAnim))
    UWidgetAnimation* WaveStateAnim;

	float HealthPercentage;
	float TotalTimePassed;
};
