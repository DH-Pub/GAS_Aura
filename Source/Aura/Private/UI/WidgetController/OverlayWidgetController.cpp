// Copyright Hung


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "AbilitySystem/Data/LevelUpDataAsset.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BindCallbacksDependencies()
{
	BindGameplayAttributeToBroadcast(AttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(AttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(AttributeSet->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(AttributeSet->GetMaxManaAttribute(), OnMaxManaChanged);

	PlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastXPToUI);
	
	if (!AbilitySystemComponent->OnGiveAbilityDelegate.IsBound())
	{
		AbilitySystemComponent->OnGiveAbilityDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastGivenAbility);
	}
	// Receive broadcast from AuraAbilitySystemComponent
	AbilitySystemComponent->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags)
	{
		for (const FGameplayTag Tag : AssetTags)
		{
			// "A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
			// if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Message"))))
			if (Tag.MatchesTag(MessageTags::Message))
			{
				MessageWidgetInfoDelegate.Broadcast(MessageInfo->FindMessageInfoForTag(Tag)); // Broadcast to BP
			}
		}
	});
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AttributeSet->GetMaxMana());

	// if AddCharacterStartupAbilities is called on the server before InitAbilityActorInfo on client
	for (const auto& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		BroadcastGivenAbility(AbilitySpec);
	}
	BroadcastXPToUI(PlayerState->GetPlayerXP(), PlayerState->GetPlayerLevel(), PlayerState->LevelUpDataAsset);
}

void UOverlayWidgetController::BroadcastGivenAbility(const FGameplayAbilitySpec& AbilitySpec)
{
	FAuraAbilityDataAsset Data = AbilityData->FindAbilityDataByTag(AbilitySpec.Ability->GetAssetTags());
	for (const FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(AuraGameplayTags::Controls)) Data.InputTag = Tag; break;
	}
	AbilityDataDelegate.Broadcast(Data);
}

void UOverlayWidgetController::BroadcastXPToUI(const int32 XP, const int32 Level, ULevelUpDataAsset* LevelUpDA) const
{
	const int32 PreLevelUpReq = LevelUpDA->LevelUpDataList[Level - 1].LevelUpRequirement;
	const int32 LevelUpReq = LevelUpDA->LevelUpDataList[Level].LevelUpRequirement;
	const float CurrentLevelXP = XP - PreLevelUpReq;
	const float DeltaLevelReq = LevelUpReq - PreLevelUpReq;
	const float CurrentLevelXPPercent = CurrentLevelXP / DeltaLevelReq;
	OnXPPercentChangedDelegate.Broadcast(Level, CurrentLevelXPPercent);
}
