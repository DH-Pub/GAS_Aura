// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraTag.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
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
	const TSubclassOf AbilityClass = FocusSpellGlobe ? FocusSpellGlobe->AbilityClass : nullptr;
	const FGameplayTag& StatusTag = FocusSpellGlobe ? FocusSpellGlobe->StatusTag : FGameplayTag();
	const bool bSpendEnabled = SpellPoints > 0 && !StatusTag.MatchesTagExact(AuraTag::Ability_Status_Locked);
	const bool bEquipEnabled = !StatusTag.MatchesTag(AuraTag::Ability_Status);

	FText Description;
	FText NextLvDescription;
	if (const FGameplayAbilitySpec* Spec = GetASC()->FindAbilitySpecFromClass(AbilityClass))
	{
		if (const UAuraGameplayAbility* AuraAbility = AbilityClass.GetDefaultObject())
		{
			FAbilityDetails Details(Spec->Level, GetASC());
			AuraAbility->GetAbilityDetails(Details);
			AuraAbility->GetDescription(Details, Description);

			FAbilityDetails ChangeDetails(Spec->Level + 1, GetASC());
			AuraAbility->GetAbilityDetails(ChangeDetails);
			AuraAbility->GetLevelChangeDescription(Details, ChangeDetails, NextLvDescription);
		}
	}
	else if (AbilityClass) // Has no Activatable Ability with Tag
	{
		if (const FAuraAbilityData* Data = UAbilityDataAsset::GetDataFromGameState(this, AbilityClass))
		{	// Description = UAuraGameplayAbility::GetLockedDescription(Data->LevelRequirement);
			Description = AuraHUD->GetLockedDescription(Data->LevelRequirement);
		}
	}
	SpellButtonFocusDelegate.Broadcast(bSpendEnabled, bEquipEnabled,
		Description, NextLvDescription, bClick);
}

void USpellMenuWidgetController::SpendPoint()
{
	if (SelectedSpellGlobe) GetASC()->ServerSpendSpellPoints(SelectedSpellGlobe->AbilityClass.GetDefaultObject());
}

bool USpellMenuWidgetController::EquipAbility()
{
	if (SelectedSpellGlobe && SelectedSpellGlobe->AbilityClass)
	{
		UpdateButtonsAndDescriptions(true);
		// Data->AbilityClass->GetDefaultObject<UAuraGameplayAbility>()->ActivationPolicy;
		return SelectedSpellGlobe->AbilityClass.GetDefaultObject()->ActivationPolicy == EAuraActivationPolicy::OnSpawn;
	}
	return false;
}

void USpellMenuWidgetController::ChangeSpellInputSlot(const TSubclassOf<UGameplayAbility> AbilityClass,
	const EAuraAbilityInputID::Type AbilityID)
{
	if (const FGameplayAbilitySpec* Spec = GetASC()->FindAbilitySpecFromClass(AbilityClass))
	{
		if (Spec->InputID == AbilityID) return; // Same Slot
		ClearSelected();
		GetASC()->ServerChangeAbilitySlot(Spec->Ability, AbilityID);
	}
}
