// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraLibrary.h"
#include "AbilitySystem/Abilities/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/SpellGlobeButtonWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	PlayerState->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPoints = Points;
		SpellPointsToUIDelegate.Broadcast(SpellPoints);
		if (FocusSpellGlobe) UpdateButtonsAndDescriptions(SpellPoints, FocusSpellGlobe->AbilityTag, FocusSpellGlobe->StatusTag);
	});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	AuraHUD->BroadcastAllActivatableAbilities();
	SpellPointsToUIDelegate.Broadcast(PlayerState->GetSpellPoints());
}

void USpellMenuWidgetController::ClearSelected()
{
	SelectedSpellGlobe = FocusSpellGlobe = nullptr;
}

void USpellMenuWidgetController::UpdateButtonsAndDescriptions(const int32 Points, const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const bool bClick)
{
	const bool bSpendEnabled = Points > 0 && !Status.MatchesTagExact(AuraGameplayTags::Ability_Status_Locked);
	const bool bEquipEnabled = Status.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked);

	FText Description;
	FText NextLvDescription;
	if (const FGameplayAbilitySpec* Spec = AbilitySystemComponent->GetSpecFromAssetTag(AbilityTag))
	{
		// Spec->GetAbilityInstances(); Spec->GetPrimaryInstance();
		if (const UCostCooldownAbility* AuraAbility = Cast<UCostCooldownAbility>(Spec->NonReplicatedInstances[0]))
		{
			FAbilityDetails Details(Spec->Level);
			AuraAbility->GetAbilityDetailsCostCooldown(Details);
			AuraAbility->GetDescription(Details, Description);

			FAbilityDetails ChangeDetails(Spec->Level + 1);
			AuraAbility->GetAbilityDetailsCostCooldown(ChangeDetails);
			AuraAbility->GetLevelChangeDescription(Details, ChangeDetails, NextLvDescription);
		}
	}
	else if (AbilityTag.IsValid()) // Has no Activatable Ability with Tag
	{
		const FAuraAbilityData* Data = UAuraLibrary::FindAbilityDataByTag(this, AbilityTag);
		// Description = UAuraGameplayAbility::GetLockedDescription(Data->LevelRequirement);
		Description = AuraHUD->GetLockedDescription(Data->LevelRequirement);
	}
	SpellButtonFocusDelegate.Broadcast(bSpendEnabled, bEquipEnabled, Description, NextLvDescription, bClick);
}

void USpellMenuWidgetController::SpendPoint()
{
	if (SelectedSpellGlobe) AbilitySystemComponent->ServerSpendSpellPoints(SelectedSpellGlobe->AbilityTag, PlayerState);
}

bool USpellMenuWidgetController::EquipAbility()
{
	if (SelectedSpellGlobe && SelectedSpellGlobe->AbilityTag.IsValid())
	{
		if (const FAuraAbilityData* Data = UAuraLibrary::FindAbilityDataByTag(this ,SelectedSpellGlobe->AbilityTag))
		{
			UpdateButtonsAndDescriptions(SpellPoints, SelectedSpellGlobe->AbilityTag, SelectedSpellGlobe->StatusTag, true);
			return Data->bIsPassive;
		}
	}
	return false;
}

void USpellMenuWidgetController::ChangeSpellInputSlot(const FGameplayTag& SlotTag, const FGameplayTag& AbilityTag,
	const bool bIsPassive)
{
	if (const FAuraAbilityData* Data = UAuraLibrary::FindAbilityDataByTag(this, AbilityTag))
	{
		if (bIsPassive != Data->bIsPassive) return;
		ClearSelected();
		AbilitySystemComponent->ServerChangeAbilitySlot(Data->AbilityTag, SlotTag);
	}
}
