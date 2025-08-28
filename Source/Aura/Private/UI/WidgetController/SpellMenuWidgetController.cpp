// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/SpellGlobeButtonWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	OverlayWC = UAuraAbilitySystemLibrary::GetOverlayWidgetController(this);
	PlayerState->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPoints = Points;
		SpellPointsToUIDelegate.Broadcast(SpellPoints);
		if (HoveredSpellGlobe) UpdateSpendEquipButtons(SpellPoints);
	});
	
	// Order: GiveAbility before Updating SpellPoints
	// AbilityDataDelegate broadcast slower than OnSpellPointChangedDelegate
	// Equipped Spell will not activate this on SpellPointsChanged
	AuraHUD->AbilityDataDelegate.AddDynamic(this, &USpellMenuWidgetController::AbilityDataUpdated);
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	AuraHUD->BroadcastAllActivatableAbilities();
	SpellPointsToUIDelegate.Broadcast(PlayerState->GetSpellPoints());
}

void USpellMenuWidgetController::ActivateSpellGlobe(USpellGlobeButtonWidget* SpellGLobeButton, const bool bClick)
{
	HoveredSpellGlobe = SpellGLobeButton;
	if (bClick && SelectedSpellGlobe != HoveredSpellGlobe)
	{
		if (SelectedSpellGlobe) SelectedSpellGlobe->Image_Selection->SetVisibility(ESlateVisibility::Hidden);
		SelectedSpellGlobe = HoveredSpellGlobe;
	}
	UpdateSpendEquipButtons(PlayerState->GetSpellPoints());
}
void USpellMenuWidgetController::ClearSelected()
{
	SelectedSpellGlobe = HoveredSpellGlobe = nullptr;
}

void USpellMenuWidgetController::UpdateSpendEquipButtons(const int32 Points)
{
	SpendPointButton->SetIsEnabled(Points > 0 && !HoveredSpellGlobe->StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Locked));
	EquipButton->SetIsEnabled(HoveredSpellGlobe->StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Equipped)
		|| HoveredSpellGlobe->StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked));

	if (RichTextDescription && RichTextNextLevel)
	{
		FText Description;
		FText NextLvDescription;
		if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromAssetTag(HoveredSpellGlobe->AbilityTag))
		{
			for (UGameplayAbility* Ability: Spec->GetAbilityInstances())
			{
				if (UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(Ability))
				{
					Description = AuraAbility->GetDescription(Spec->Level, 0);
					NextLvDescription = AuraAbility->GetDescription(Spec->Level, 1);
					break;
				}
			}
		}
		else if (HoveredSpellGlobe->AbilityTag.IsValid()) // Has no Activatable Ability with Tag
		{
			const FAuraAbilityData* Data = AuraHUD->AbilityData->FindAbilityDataByTags(HoveredSpellGlobe->AbilityTag);
			// Description = UAuraGameplayAbility::GetLockedDescription(Data->LevelRequirement);
			Description = AuraHUD->GetLockedDescription(Data->LevelRequirement);
		}
		RichTextDescription->SetText(Description);
		RichTextNextLevel->SetText(NextLvDescription);
	}
}

void USpellMenuWidgetController::SpendPoint()
{
	if (SelectedSpellGlobe) AbilitySystemComponent->ServerSpendSpellPoints(SelectedSpellGlobe->AbilityTag);
}

UOverlayWidgetController* USpellMenuWidgetController::GetOverlayWC()
{
	if (OverlayWC == nullptr) OverlayWC = UAuraAbilitySystemLibrary::GetOverlayWidgetController(this);
	return OverlayWC;
}

void USpellMenuWidgetController::AbilityDataUpdated(const FAuraAbilityData& Data)
{
	if (HoveredSpellGlobe && Data.AbilityTag.MatchesTagExact(HoveredSpellGlobe->AbilityTag))
	{
		HoveredSpellGlobe->StatusTag = Data.StatusTag;
		UpdateSpendEquipButtons(SpellPoints);
	}
}
