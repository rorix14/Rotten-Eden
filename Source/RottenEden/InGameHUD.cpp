#include "InGameHUD.h"
#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "REMedkitUI.h"
#include "Kismet/GameplayStatics.h"

bool UInGameHUD::Initialize()
{
	const bool Success = Super::Initialize();
	if (!Success) return false;

	HealthPercentage = 1;
	TotalTimePassed = 0;

	for (uint16 i = 0; i < NumberOfMedkits; i++)
	{
		UREMedkitUI* Medkit = CreateWidget<UREMedkitUI>(this, MedkitUI);
		Medkit->SetVisibility(ESlateVisibility::Visible);
		Medkit->SetRenderOpacity(1.0f);
		MedkitContainer->AddChild(Medkit);
	}

	return true;
}

void UInGameHUD::NativeConstruct() { Super::NativeConstruct(); }

void UInGameHUD::BeginDestroy() { Super::BeginDestroy(); }

void UInGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TotalTimePassed += InDeltaTime;
	const float TargetRange = 1 - HealthPercentage - 0.2f;
	const float EndRange = TargetRange - 0.3f;
	const float OscillationRange = (TargetRange - EndRange) / 2;
	const float OscillationOffset = OscillationRange + EndRange;
	DamageVignette->SetOpacity(FMath::Lerp(DamageVignette->ColorAndOpacity.A,
	                                       OscillationOffset + FMath::Sin(TotalTimePassed * 3.0f) * OscillationRange,
	                                       InDeltaTime * 3.0f));
}

void UInGameHUD::UpdateHealthUI(const float& Health, const float& MaxHealth, const float& DamageTaken)
{
	HealthText->SetText(FText::FromString(FString::FromInt(static_cast<int>(Health))));
	if (DamageTaken != 0)
		HealthTextAnimValue->SetText(FText::FromString(FString::FromInt(static_cast<int>(DamageTaken))));

	HealthPercentage = Health / MaxHealth;
	HealthBar->SetPercent(HealthPercentage);
	
	const FLinearColor RedImage((HealthPercentage <= 0.3f ? FColor::Red : FColor::White));
	HealthBar->SetFillColorAndOpacity(RedImage);
	HealthIcon->SetColorAndOpacity(RedImage);
	HealthText->SetColorAndOpacity(RedImage);
	HealthTextAnimValue->SetColorAndOpacity(RedImage);
}

void UInGameHUD::UpdateAmmoUI(const int16& MagazineAmmo, const int16& TotalAmmo) const
{
	AmmoInMagazineText->SetText(FText::FromString(FString::FromInt(MagazineAmmo)));
	TotalAmmoText->SetText(FText::FromString(FString::FromInt(TotalAmmo)));
}

void UInGameHUD::UpdateCreditsUI(const int16& CreditsDelta, const int16& TotalCredits)
{
	TotalCreditsText->SetText(FText::FromString(FString::FromInt(static_cast<int>(TotalCredits))));
	DeltaCreditsText->SetText(FText::FromString((CreditsDelta > 0
		                                             ? TEXT("+")
		                                             : TEXT("")) + FString::FromInt(static_cast<int>(CreditsDelta))));
	PlayAnimation(CreditsTextAnim);
}

void UInGameHUD::SetWeaponImage(UTexture2D* Image) const
{
	GunImage->SetBrushFromTexture(Image);
}

void UInGameHUD::SetWeaponScopeImage(UTexture2D* Image, ESlateVisibility&& ScopeVisibility) const
{
	WeaponScope->SetBrushFromTexture(Image);
	WeaponScope->SetVisibility(ScopeVisibility);
}

void UInGameHUD::ConsumeMedkitUI()
{
	for (int16 i = MedkitContainer->GetChildrenCount(); i-- > 0;)
	{
		UREMedkitUI* Medkit = Cast<UREMedkitUI>(MedkitContainer->GetChildAt(i));
		if (!Medkit->HasUsed)
		{
			Medkit->PlayAnimation(Medkit->GetUseMedkitAnim());
			Medkit->HasUsed = true;
			return;
		}
	}
}

void UInGameHUD::AddMedkitUI()
{
	for (int i = 0; i < MedkitContainer->GetChildrenCount(); ++i)
	{
		UREMedkitUI* Medkit = Cast<UREMedkitUI>(MedkitContainer->GetChildAt(i));
		if (Medkit->HasUsed)
		{
			Medkit->PlayAnimation(Medkit->GetUseMedkitAnim(), 0.0f, 1, EUMGSequencePlayMode::Reverse);
			Medkit->HasUsed = false;
			return;
		}
	}
}

void UInGameHUD::ShowObjectiveDestroyedNotification(const int16& RemainingObjectives)
{
	if (RemainingObjectives > 1)
	{
		RemainingText->SetVisibility(ESlateVisibility::Visible);
		RemainingTentsText->SetText(FText::FromString(
			TEXT("Defend the remaining ") + FString::FromInt(RemainingObjectives) + TEXT(" tents")));
	}
	else
	{
		RemainingText->SetVisibility(ESlateVisibility::Collapsed);
		RemainingTentsText->SetText(FText::FromString(RemainingObjectives == 1
			                                              ? TEXT("Defend the last remaining medical tent")
			                                              : TEXT("All medical tents have been destoyed")));
	}

	UGameplayStatics::PlaySound2D(GetWorld(), OnDestroyedObjectiveNotificationSound);
	PlayAnimation(TentDestroyedPromptAnim);
}

void UInGameHUD::WaveSateNotification(const int32& WaveNumber, const bool& WaveHasEnded)
{
	if (WaveHasEnded)
	{
		WaveStateText->SetText(FText::FromString("Wave " + FString::FromInt(WaveNumber) + " Completed"));
		UGameplayStatics::SpawnSound2D(GetWorld(), OnWaveCompletedSound);
	}
	else
	{
		WaveStateText->SetText(FText::FromString("Wave " + FString::FromInt(WaveNumber) + " Started"));
		UGameplayStatics::SpawnSound2D(GetWorld(), OnStartedSound);
	}

	PlayAnimation(WaveStateAnim);
}
