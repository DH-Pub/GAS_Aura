// Copyright Hung


#include "AbilitySystem/Abilities/AuraListenForEventAbility.h"

#include "AbilitySystemComponent.h"

UAuraListenForEventAbility::UAuraListenForEventAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UAuraListenForEventAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	for (const FGameplayModifierInfo& Mod : EventBasedEffectClass->GetDefaultObject<UGameplayEffect>()->Modifiers)
	{
		ModifiersDataTags.AddTag(Mod.ModifierMagnitude.GetSetByCallerFloat().DataTag);
	}
}

void UAuraListenForEventAbility::ApplyEventEffectsToSelf(const FGameplayEventData& Payload)
{
	// UAuraAbilitySystemComponent* ASC = AuraCharacterFromActorInfo->GetAuraAbilitySystemComponent();
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(EventBasedEffectClass, 1.f, FGameplayEffectContextHandle());
	FGameplayEffectSpec* Spec = EffectSpecHandle.Data.Get();
	if (Spec == nullptr) return;
	for (const FGameplayTag& Tag : ModifiersDataTags)
	{
		const float Magnitude = Tag.MatchesTagExact(Payload.EventTag) ? Payload.EventMagnitude : 0.f;
		// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(EffectSpecHandle, Tag, Payload.EventMagnitude)
		// Spec->SetSetByCallerMagnitude(Tag, Magnitude);
		Spec->SetByCallerTagMagnitudes.FindOrAdd(Tag) = Magnitude;
	}
	ASC->ApplyGameplayEffectSpecToSelf(*Spec);
}
