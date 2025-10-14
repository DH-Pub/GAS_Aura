// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/SpellGlobeButtonWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	GetPlayerState()->OnSpellPointsChangedDelegate.RemoveAll(this);
	GetPlayerState()->OnSpellPointsChangedDelegate.AddLambda([&](const int32 Points)
	{
		SpellPoints = Points;
		SpellPointsToUIDelegate.Broadcast(SpellPoints);
		if (FocusSpellGlobe) UpdateButtonsAndDescriptions(SpellPoints, FocusSpellGlobe->AbilityTag, FocusSpellGlobe->StatusTag);
	});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	AuraHUD->BroadcastAllActivatableAbilities();
	SpellPointsToUIDelegate.Broadcast(GetPlayerState()->GetSpellPoints());
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
	if (const FGameplayAbilitySpec* Spec = GetASC()->GetSpecFromAssetTag(AbilityTag))
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
		if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(this, AbilityTag))
		{	// Description = UAuraGameplayAbility::GetLockedDescription(Data->LevelRequirement);
			Description = AuraHUD->GetLockedDescription(Data->LevelRequirement);
		}
	}
	SpellButtonFocusDelegate.Broadcast(bSpendEnabled, bEquipEnabled,
		Description, NextLvDescription, bClick);
}

void USpellMenuWidgetController::SpendPoint()
{
	if (SelectedSpellGlobe) GetASC()->ServerSpendSpellPoints(SelectedSpellGlobe->AbilityTag);
}

bool USpellMenuWidgetController::EquipAbility()
{
	if (SelectedSpellGlobe && SelectedSpellGlobe->AbilityTag.IsValid())
	{
		if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(
			this, SelectedSpellGlobe->AbilityTag))
		{
			UpdateButtonsAndDescriptions(SpellPoints, SelectedSpellGlobe->AbilityTag, SelectedSpellGlobe->StatusTag, true);
			return Data->AbilityClass->GetDefaultObject<UAuraGameplayAbility>()->bActivateAbilityOnGranted;
		}
	}
	return false;
}

void USpellMenuWidgetController::ChangeSpellInputSlot(const FGameplayTag& AbilityTag, const FGameplayTag NewSlotTag,
	const bool bIsPassive)
{
	if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(this, AbilityTag))
	{
		// Check Wrong type (Active != Passive Slots)
		if (bIsPassive != Data->AbilityClass->GetDefaultObject<UAuraGameplayAbility>()->bActivateAbilityOnGranted) return;
		ClearSelected();
		GetASC()->ServerChangeAbilitySlot(AbilityTag, NewSlotTag);
	}
}
