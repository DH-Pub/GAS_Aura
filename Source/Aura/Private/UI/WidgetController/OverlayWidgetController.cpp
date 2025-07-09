// Copyright Hung


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"

void UOverlayWidgetController::BindCallbacksDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetMaxManaAttribute(), OnMaxManaChanged);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (!AuraASC->AbilitiesGivenDelegate.IsBound())
		{
			AuraASC->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityData);
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
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BroadcastAbilityData(UAuraAbilitySystemComponent* AuraASC)
{
	//TODO: Get info about all given abilities, broadcast them
	FForEachAbility AbilityDelegate;
	AbilityDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		FAuraAbilityDataAsset Data = AbilityData->FindAbilityDataByTag(AbilitySpec.Ability->GetAssetTags());
		for (FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(AuraGameplayTags::Input)) Data.InputTag = Tag; break;
		}
		AbilityDataDelegate.Broadcast(Data);
	});
	AuraASC->ForEachAbility(AbilityDelegate); // Execute the above lambda with ASC GetActivatableAbilities()
}
