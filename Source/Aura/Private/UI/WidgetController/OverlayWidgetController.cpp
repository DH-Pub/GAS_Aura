// Copyright Hung


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BindCallbacksDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetHealthAttribute(), OnHealthChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetMaxHealthAttribute(), OnMaxHealthChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetManaAttribute(), OnManaChanged);
	BindGameplayAttributeToBroadcast(AuraAttributeSet->GetMaxManaAttribute(), OnMaxManaChanged);
	
	// Receive broadcast from AuraAbilitySystemComponent
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag Tag : AssetTags)
			{
				// "A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
				if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Message"))))
				{
					const FAuraMessageInfo Info = MessageInfo->FindMessageInfoForTag(Tag);
					MessageWidgetInfoDelegate.Broadcast(Info); // Broadcast to BP
				}
			}
		}
	);
}

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}
