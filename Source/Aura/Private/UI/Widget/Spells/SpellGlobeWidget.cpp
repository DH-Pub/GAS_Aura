// Copyright Hung


#include "UI/Widget/Spells/SpellGlobeWidget.h"

#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AbilitySystem/AsyncTask/Async_CooldownChange.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellGlobeWidget::SetWidgetController(UAuraWidgetController* InWidgetController)
{
	OverlayWC = Cast<UOverlayWidgetController>(InWidgetController); check(OverlayWC);
	WheelMaterialInstance = Image_WheelProgress->GetDynamicMaterial();
	Image_WheelProgress->SetVisibility(ESlateVisibility::Collapsed);
	Super::SetWidgetController(InWidgetController);
}

void USpellGlobeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Cast<UOverlaySlot>(Progress_Cooldown->Slot)->SetPadding(InPadding);
	Cast<UOverlaySlot>(Image_SpellIcon->Slot)->SetPadding(InPadding);

	Progress_Cooldown->SetBarFillType(EProgressBarFillType::TopToBottom);
	ClearGlobe();
}

void USpellGlobeWidget::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	if (WaitCDTask) WaitCDTask->EndTask();
}

bool USpellGlobeWidget::SuccessUpdateAbilityData(const FAuraAbilityData& InAbilityData, const FPlayerAbilityData& InPlayerData,
	FGameplayTagContainer& OutCooldownTags)
{
	if (AbilityID != InPlayerData.AbilityID)
	{	// Not this Slot
		if (AbilityTag.MatchesTagExact(InAbilityData.GetAuraAbilityTag()))
		{	// Ability saved to this Slot is moved out
			if (WaitCDTask) WaitCDTask->EndTask(); // Clear any active Task when Ability Changed
			ClearGlobe();
		}
		return false; // Do not clear Task here so other Data won't clear Task of others
	}
	if (WaitCDTask) WaitCDTask->EndTask();
	// Update AbilityTag to compare with next InAbilityData when SuccessUpdateAbilityData called
	AbilityTag = InAbilityData.GetAuraAbilityTag();

	FSlateBrush ResourceImage;
	ResourceImage.SetResourceObject(InAbilityData.Icon);
	Image_SpellIcon->SetBrush(ResourceImage);
	ResourceImage.SetResourceObject(InAbilityData.BackgroundMaterial);
	Image_Background->SetBrush(ResourceImage);
	// Image_Background->SetBrushFromMaterial(InAbilityData.BackgroundMaterial);

	if (const FGameplayTagContainer* CooldownTags = InAbilityData.AbilityClass.GetDefaultObject()->GetCooldownTags())
	{
		OutCooldownTags = *CooldownTags;
		if (!OutCooldownTags.IsValid()) return false;
	}
	return true;
}

void USpellGlobeWidget::UpdateCooldown(const float InTime, const float InDuration)
{
	if (!bOnCooldown) // Gray out Ability
	{
		Image_Background->SetBrushTintColor(FSlateColor(FLinearColor(.1f, .1f, .1f, .5f)));
		Image_WheelProgress->SetVisibility(ESlateVisibility::Visible);
		WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, 1.f);
	}
	if (InDuration < -UE_KINDA_SMALL_NUMBER) return; // InDuration is -1.f on client: wait for server correction
	TimeRemaining = InTime;
	CooldownDuration = InDuration;

	const float CooldownPercent = TimeRemaining / CooldownDuration;
	Progress_Cooldown->SetPercent(CooldownPercent);
	WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, CooldownPercent);

	bOnCooldown = true;
	Text_Cooldown->SetVisibility(ESlateVisibility::Visible);
	Text_Cooldown->SetText(UKismetTextLibrary::Conv_DoubleToText(TimeRemaining, HalfToEven, false,
		true, 1, 2, 1, 1));
	if (const UWorld* World = GetWorld())
	{	/*FTimerDelegate Delegate; Delegate.BindUFunction(this, "UpdateByTimerHandle", DelegateParameter);*/
		World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &USpellGlobeWidget::UpdateByTimerHandle,
			Frequency, true);
	}
}

void USpellGlobeWidget::EndCooldown()
{
	bOnCooldown = false;
	if (AbilityTag.IsValid()) Image_Background->SetBrushTintColor(FSlateColor(FLinearColor::White));
	Progress_Cooldown->SetPercent(0.f);
	Image_WheelProgress->SetVisibility(ESlateVisibility::Collapsed);
	Text_Cooldown->SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
}

// Function for CooldownTimerHandle
void USpellGlobeWidget::UpdateByTimerHandle()
{
	TimeRemaining = FMath::Clamp(TimeRemaining - Frequency, 0.f, CooldownDuration);
	if (TimeRemaining < UE_KINDA_SMALL_NUMBER)
	{
		EndCooldown(); return;
	}
	const float CooldownPercent = TimeRemaining / CooldownDuration;
	Progress_Cooldown->SetPercent(CooldownPercent);
	WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, CooldownPercent);
	Text_Cooldown->SetText(UKismetTextLibrary::Conv_DoubleToText(TimeRemaining, HalfToEven, false, true,
		1, 2, 1, 1));
}

void USpellGlobeWidget::ClearGlobe()
{
	AbilityTag = FGameplayTag::EmptyTag;
	FSlateBrush ClearBrush = FSlateBrush();
	ClearBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.f));
	Image_SpellIcon->SetBrush(ClearBrush);
	Image_Background->SetBrush(ClearBrush);
	EndCooldown();
}
