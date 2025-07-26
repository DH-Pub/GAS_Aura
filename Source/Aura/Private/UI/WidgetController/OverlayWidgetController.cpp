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
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetMaxManaAttribute(), OnMaxManaChanged);

	AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(PlayerState);
	AuraPS->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastXPToUI);
	
	UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	if (!AuraASC->OnGiveAbilityDelegate.IsBound())
	{
		AuraASC->OnGiveAbilityDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastGivenAbility);
	}
	// Receive broadcast from AuraAbilitySystemComponent
	AuraASC->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags)
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
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

	// if AddCharacterStartupAbilities is called on the server before InitAbilityActorInfo on client
	for (const auto& AbilitySpec : AbilitySystemComponent->GetActivatableAbilities())
	{
		BroadcastGivenAbility(AbilitySpec);
	}
	if (AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(PlayerState))
	{
		BroadcastXPToUI(AuraPS->GetPlayerXP(), AuraPS->GetPlayerLevel(), AuraPS->LevelUpDataAsset);
	}
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
