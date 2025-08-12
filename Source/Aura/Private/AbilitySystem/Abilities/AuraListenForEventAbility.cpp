// Copyright Hung


#include "AbilitySystem/Abilities/AuraListenForEventAbility.h"

#include "AbilitySystemComponent.h"

void UAuraListenForEventAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,const FGameplayAbilityActivationInfo ActivationInfo,
                                             FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	for (const FGameplayModifierInfo& Mod : EventBasedEffectClass->GetDefaultObject<UGameplayEffect>()->Modifiers)
	{
		ModifiersDataTags.AddTag(Mod.ModifierMagnitude.GetSetByCallerFloat().DataTag);
	}
}

void UAuraListenForEventAbility::ApplyEventEffectsToSelf(const FGameplayEventData& Payload)
{
	// UAuraAbilitySystemComponent* ASC = AuraCharacterFromActorInfo->GetAuraAbilitySystemComponent();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(EventBasedEffectClass, 1.f, FGameplayEffectContextHandle());
	FGameplayEffectSpec* Spec = EffectSpecHandle.Data.Get();
	if (Spec == nullptr) return;
	for (const FGameplayTag& Tag : ModifiersDataTags)
	{
		// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tag, Payload.EventMagnitude)
		const float Magnitude = Tag.MatchesTagExact(Payload.EventTag) ? Payload.EventMagnitude : 0.f;
		Spec->SetSetByCallerMagnitude(Tag, Magnitude);
	}
	ASC->ApplyGameplayEffectSpecToSelf(*Spec);
}
