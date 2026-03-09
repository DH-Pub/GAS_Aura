// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/Spells/SpellGlobeButtonWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	GetPlayerState()->OnSpellPointsChangedDelegate.RemoveAll(this);
	GetPlayerState()->OnSpellPointsChangedDelegate.AddWeakLambda(this, [this](const int32 Points)
	{
		SpellPoints = Points;
		SpellPointsToUIDelegate.Broadcast(SpellPoints);
		if (FocusSpellGlobe) UpdateButtonsAndDescriptions();
	});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	GetASC()->BroadcastAllAbilityData();
	SpellPointsToUIDelegate.Broadcast(GetPlayerState()->GetSpellPoints());
}

void USpellMenuWidgetController::ClearSelected()
{
	SelectedSpellGlobe = FocusSpellGlobe = nullptr;
}

void USpellMenuWidgetController::UpdateButtonsAndDescriptions(const bool bClick) const
{
	const FGameplayTag& AbilityTag = FocusSpellGlobe ? FocusSpellGlobe->AbilityTag : FGameplayTag();
	const FGameplayTag& StatusTag = FocusSpellGlobe ? FocusSpellGlobe->StatusTag : FGameplayTag();
	const bool bSpendEnabled = SpellPoints > 0 && !StatusTag.MatchesTagExact(AuraGameplayTags::Ability_Status_Locked);
	const bool bEquipEnabled = !StatusTag.MatchesTag(AuraGameplayTags::Ability_Status);

	FText Description;
	FText NextLvDescription;
	if (const FGameplayAbilitySpec* Spec = GetASC()->GetSpecFromAbilityTag(AbilityTag))
	{
		// Spec->GetAbilityInstances(); Spec->GetPrimaryInstance();
		if (const UCostCooldownAbility* AuraAbility = Cast<UCostCooldownAbility>(Spec->NonReplicatedInstances[0]))
		{
			FAbilityDetails Details(Spec->Level);
			AuraAbility->GetAbilityDetails(Details);
			AuraAbility->GetDescription(Details, Description);

			FAbilityDetails ChangeDetails(Spec->Level + 1);
			AuraAbility->GetAbilityDetails(ChangeDetails);
			AuraAbility->GetLevelChangeDescription(Details, ChangeDetails, NextLvDescription);
		}
	}
	else if (AbilityTag.IsValid()) // Has no Activatable Ability with Tag
	{
		if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(this,
			FGameplayTagContainer(AbilityTag)))
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
		if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(this,
			FGameplayTagContainer(SelectedSpellGlobe->AbilityTag)))
		{
			UpdateButtonsAndDescriptions(true);
			return Data->AbilityClass->GetDefaultObject<UAuraGameplayAbility>()->ActivationPolicy == EAuraActivationPolicy::OnSpawn;
		}
	}
	return false;
}

void USpellMenuWidgetController::ChangeSpellInputSlot(const FGameplayTag& AbilityTag,
	const EAuraAbilityInputID::Type AbilityID)
{
	if (const FGameplayAbilitySpec* Spec = GetASC()->GetSpecFromAbilityTag(AbilityTag))
	{
		if (Spec->InputID == AbilityID) return; // Same Slot

		ClearSelected();
		GetASC()->ServerChangeAbilitySlot(AbilityTag, AbilityID);
	}
}
