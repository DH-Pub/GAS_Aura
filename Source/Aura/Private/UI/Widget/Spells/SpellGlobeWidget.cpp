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
	OverlayWC->GetASC()->AbilityDataDelegate.AddDynamic(this, &USpellGlobeWidget::SuccessUpdateAbilityData);
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
	if (OverlayWC) OverlayWC->GetASC()->AbilityDataDelegate.RemoveAll(this);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
	Super::NativeDestruct();
}

void USpellGlobeWidget::SuccessUpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec, const FAuraAbilityData& Data)
{
	if (AbilityID != AbilitySpec.InputID)
	{	// Not this Slot
		if (AbilityClass == Data.AbilityClass)
		{	// Ability saved to this Slot is moved out
			ClearGlobe();
		}
		return;
	}
	AbilityClass = Data.AbilityClass; // Update Class to compare with next FAuraAbilityData

	FSlateBrush ResourceObj; ResourceObj.SetResourceObject(Data.Icon);
	Image_SpellIcon->SetBrush(ResourceObj);
	ResourceObj.SetResourceObject(Data.BackgroundMaterial);
	Image_Background->SetBrush(ResourceObj); //Image_Background->SetBrushFromMaterial(Data.BackgroundMaterial);

	if (const FGameplayTagContainer* Tags = AbilitySpec.GetPrimaryInstance()->GetCooldownTags())
	{	// if (const FGameplayTagContainer* Tags = Data.AbilityClass.GetDefaultObject()->GetCooldownTags())
		if (Tags->Num() > 0) // Check if Ability has Cooldown
		{
			CooldownTags = *Tags;
			CheckAbilityCooldown();
		}
	}
}

void USpellGlobeWidget::CheckAbilityCooldown()
{
	const UAuraAbilitySystemComponent* ASC = OverlayWC->GetASC();
	const bool bIsAuth = ASC->GetOwnerRole() == ROLE_Authority;
	const float CurrentTime = ASC->GetWorld()->GetTimeSeconds();
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
	TimeRemaining = CooldownDuration = 0.f;
	for (const FActiveGameplayEffect& Effect : &ASC->GetActiveGameplayEffects())
	{
		if (!Query.Matches(Effect)) continue;
		// _NotReplicated() will return null for Received data from others (Server), != nullptr for Local
		const bool bIsServerCorrection = Effect.Spec.GetContext().GetAbilityInstance_NotReplicated() == nullptr;
		if (bIsAuth /*Is Server*/ ||
			bIsServerCorrection /* Client Receiving Server Correction*/)
		{
			const float Duration = Effect.GetDuration();
			const float Remain = Duration - (CurrentTime - Effect.StartWorldTime);
			if (Remain > TimeRemaining)
			{	// We choose the longest cooldown
				TimeRemaining = Remain;
				CooldownDuration = Duration;
			}
		}
		else if (!bIsServerCorrection) // Not using Local Prediction
		{
			TimeRemaining = CooldownDuration = -1.f;
		}
	}

	// UpdateCooldown()
	if (FMath::IsNearlyZero(TimeRemaining * CooldownDuration))
	{
		EndCooldown();
		return;
	}

	// Wait for Server correction
	Image_Background->SetBrushTintColor(FSlateColor(FLinearColor(.2f, .2f, .2f))); // Gray out Ability
	Image_WheelProgress->SetVisibility(ESlateVisibility::Visible);

	if (CooldownDuration < 0.f)
	{	// CooldownDuration is -1.f on client: wait for server correction
		WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, 1.f);
		return;
	}

	const float CooldownPercent = TimeRemaining / CooldownDuration;
	Progress_Cooldown->SetPercent(CooldownPercent);
	WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, CooldownPercent);

	Text_Cooldown->SetVisibility(ESlateVisibility::Visible);
	Text_Cooldown->SetText(UKismetTextLibrary::Conv_DoubleToText(TimeRemaining, HalfToEven, false,
		true, 1, 2, 1, 1));
	if (const UWorld* World = GetWorld())
	{	/*FTimerDelegate Delegate; Delegate.BindUFunction(this, "UpdateByTimerHandle", DelegateParameter);*/
		World->GetTimerManager().ClearTimer(CooldownTimerHandle);
		World->GetTimerManager().SetTimer(CooldownTimerHandle, this, &USpellGlobeWidget::UpdateByTimerHandle,
			Frequency, true);
	}
}

void USpellGlobeWidget::EndCooldown()
{
	if (AbilityClass) Image_Background->SetBrushTintColor(FSlateColor(FLinearColor::White));
	Progress_Cooldown->SetPercent(0.f);
	Image_WheelProgress->SetVisibility(ESlateVisibility::Collapsed);
	Text_Cooldown->SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
}

void USpellGlobeWidget::ClearGlobe()
{
	Super::ClearGlobe();

	CooldownTags.Reset();
	FSlateBrush ClearBrush = FSlateBrush();
	ClearBrush.TintColor = FSlateColor(FLinearColor(1.f, 1.f, 1.f, 0.f));
	Image_SpellIcon->SetBrush(ClearBrush);
	Image_Background->SetBrush(ClearBrush);
	EndCooldown();
}

// Function for CooldownTimerHandle
void USpellGlobeWidget::UpdateByTimerHandle()
{
	if (!OverlayWC->GetASC()->HasAnyMatchingGameplayTags(CooldownTags))
	{
		EndCooldown();
		return;
	}
	TimeRemaining = FMath::Clamp(TimeRemaining - Frequency, 0.f, CooldownDuration);
	const float CooldownPercent = TimeRemaining / CooldownDuration;
	Progress_Cooldown->SetPercent(CooldownPercent);
	WheelMaterialInstance->SetScalarParameterValue(WheelPercentParam, CooldownPercent);
	Text_Cooldown->SetText(UKismetTextLibrary::Conv_DoubleToText(TimeRemaining, HalfToEven, false,
		true, 1, 3, 1, 1));
}
