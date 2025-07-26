// Copyright Hung


#include "UI/Widget/SpellGlobeWidget.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellGlobeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	Image_WheelProgress->SetBrush(WheelBrush);
	Cast<UOverlaySlot>(Progress_Cooldown->Slot)->SetPadding(InPadding);
	Cast<UOverlaySlot>(Image_SpellIcon->Slot)->SetPadding(InPadding);
	
	Progress_Cooldown->SetBarFillType(EProgressBarFillType::TopToBottom);
	
	// Clear Globe
	FSlateBrush ClearBrush = FSlateBrush();
	ClearBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.f));
	Image_SpellIcon->SetBrush(ClearBrush);
	Progress_Cooldown->SetPercent(0.f);
	Text_Cooldown->SetRenderOpacity(0.f);
	DisableTint = 0.1f;
}

void USpellGlobeWidget::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
}

void USpellGlobeWidget::WidgetControllerSet_Implementation(UAuraWidgetController* Controller)
{
	OverlayWC = Cast<UOverlayWidgetController>(Controller);
	WheelMaterialInstance = Image_WheelProgress->GetDynamicMaterial();
	WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, 0.f);
	Image_WheelProgress->SetRenderOpacity(0.f);
}

void USpellGlobeWidget::AbilityDataUpdate(TEnumAsByte<EOutcome>& Outcome, const FAuraAbilityDataAsset& InAbilityData)
{
	Outcome = EOutcome::Failure;
	if(InputTag.MatchesTagExact(InAbilityData.InputTag))
	{
		CooldownTag = InAbilityData.CooldownTag;
		FSlateBrush ResourceImage;
		ResourceImage.SetResourceObject(InAbilityData.Icon);
		Image_SpellIcon->SetBrush(ResourceImage);
		FSlateBrush BackgroundMaterial;
		BackgroundMaterial.SetResourceObject(InAbilityData.BackgroundMaterial);
		Image_Background->SetBrush(BackgroundMaterial);
		
		Outcome = EOutcome::Success;
	}
}

void USpellGlobeWidget::UpdateCooldown(const float InTime, const float InDuration)
{
	if (!bOnCooldown)
	{
		Image_Background->SetBrushTintColor(FSlateColor(FLinearColor(DisableTint, DisableTint, DisableTint, 1.f)));
		Image_WheelProgress->SetRenderOpacity(1.f);
		WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, 1.f);
	}
	if (InDuration < 0.f) return; // InDuration is -1.f on client and wait for server correction
	TimeRemaining = InTime;
	CooldownDuration = InDuration;
	
	const float CooldownPercent = TimeRemaining / CooldownDuration;
	Progress_Cooldown->SetPercent(CooldownPercent);
	WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, CooldownPercent);

	if (!bOnCooldown)
	{
		bOnCooldown = true;
		Text_Cooldown->SetRenderOpacity(1.f);
	}
	Text_Cooldown->SetText(UKismetTextLibrary::Conv_DoubleToText(TimeRemaining, HalfToEven, false, true,
	1, 2, 1, 1));
	if (const UWorld* World = GetWorld())
	{
		/*FTimerDelegate Delegate; Delegate.BindUFunction(this, "UpdateByTimerHandle", InTime);*/
		World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &USpellGlobeWidget::UpdateByTimerHandle, Frequency, true);
	}
}

void USpellGlobeWidget::EndCooldown()
{
	bOnCooldown = false;
	Image_Background->SetBrushTintColor(FSlateColor(FLinearColor::White));
	Progress_Cooldown->SetPercent(0.f);
	WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, 0.f);
	Image_WheelProgress->SetRenderOpacity(0.f);
	Text_Cooldown->SetRenderOpacity(0.f);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
}

void USpellGlobeWidget::UpdateByTimerHandle()
{
	TimeRemaining = FMath::Clamp(TimeRemaining - Frequency, 0.f, CooldownDuration);
	if (TimeRemaining > 0.f)
	{
		const float CooldownPercent = TimeRemaining / CooldownDuration;
		Progress_Cooldown->SetPercent(CooldownPercent);
		WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, CooldownPercent);
		Text_Cooldown->SetText(UKismetTextLibrary::Conv_DoubleToText(TimeRemaining, HalfToEven, false, true,
			1, 2, 1, 1));
	}
	else
	{
		WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, 0.f);
		Progress_Cooldown->SetPercent(0.f);
		Text_Cooldown->SetRenderOpacity(0.f);
	}
}

void USpellGlobeWidget::ClearGlobe()
{
	FSlateBrush ClearBrush = FSlateBrush();
	ClearBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.f));
	Image_SpellIcon->SetBrush(ClearBrush);
	Image_Background->SetBrush(ClearBrush);
}
