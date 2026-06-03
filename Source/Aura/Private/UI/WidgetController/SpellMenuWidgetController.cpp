// Copyright Hung


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraTag.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/Spells/SpellGlobeButtonWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

void USpellMenuWidgetController::UnbindOldAbilitySystemComponent()
{
	AuraASC->AbilityDataDelegate.RemoveAll(this);
	if (AAuraPlayerState* OldPS = GetPlayerState())
	{
		OldPS->OnSpellPointsChangedDelegate.RemoveAll(this);
	}
}

void USpellMenuWidgetController::BindCallbacksDependencies()
{
	AuraASC->AbilityDataDelegate.AddWeakLambda(this, [this]()
	{
		OnReceiveAbilityDataFromASC.Broadcast();
	});
	GetPlayerState()->OnSpellPointsChangedDelegate.AddWeakLambda(this, [this](const int32 Points)
	{
		GetASC()->BroadcastAllAbilityData();
		SpellPointsToUIDelegate.Broadcast(SpellPoints = Points);
		if (FocusSpellGlobe) UpdateButtonsAndDescriptions();
	});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	if (GetPlayerState()) GetPlayerState()->BroadcastCurrentData();
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
	if (const FGameplayAbilitySpec* Spec = AuraASC->FindAbilitySpecFromClass(AbilityClass))
	{
		if (const UAuraGameplayAbility* AuraAbility = AbilityClass.GetDefaultObject())
		{
			FAbilityDetails Details(Spec->Level, AuraASC);
			AuraAbility->GetAbilityDetails(Details);
			AuraAbility->GetDescription(Details, Description);

			FAbilityDetails ChangeDetails(Spec->Level + 1, AuraASC);
			AuraAbility->GetAbilityDetails(ChangeDetails);
			AuraAbility->GetLevelChangeDescription(Details, ChangeDetails, NextLvDescription);
		}
	}
	else if (AbilityClass) // Has no Activatable Ability with Tag
	{
		if (const FAuraAbilityData* Data = UAbilityDataAsset::GetDataFromGameState(this, AbilityClass))
		{	// Description = UAuraGameplayAbility::GetLockedDescription(Data->LevelRequirement);
			Description = AAuraHUD::Get(this)->GetLockedDescription(Data->LevelRequirement);
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
		return SelectedSpellGlobe->AbilityClass.GetDefaultObject()->ActivationPolicy == EAuraActivationPolicy::OnSpawn;
	}
	return false;
}

void USpellMenuWidgetController::ChangeSpellInputSlot(const TSubclassOf<UGameplayAbility> AbilityClass,
	const EAuraAbilityInputID::Type AbilityID)
{
	GetASC()->ClearInput();
	if (const FGameplayAbilitySpec* Spec = GetASC()->FindAbilitySpecFromClass(AbilityClass))
	{
		if (Spec->InputID == AbilityID)
		{	// Same Slot
			GetASC()->BroadcastAllAbilityData();
			return;
		}
		ClearSelected();
		GetASC()->ServerChangeAbilitySlot(Spec->Ability, AbilityID);
	}
}
