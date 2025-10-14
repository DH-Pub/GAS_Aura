// Copyright Hung


#include "AbilitySystem/Ability/AttributesEventAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

UAttributeEventEffect::UAttributeEventEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FSetByCallerFloat SetByCallerFloat;
	FGameplayModifierInfo Info;
#define ADD_EFFECT_MODIFIER(Modifiers, Property, Op, Tag)\
{\
	Info.Attribute = UAuraAttributeSet::Property;\
	Info.ModifierOp = EGameplayModOp::Op;\
	SetByCallerFloat.DataTag = AuraGameplayTags::Tag;\
	Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerFloat);\
	Modifiers.Add(Info);\
}
	ADD_EFFECT_MODIFIER(Modifiers, GetIncomingXPAttribute(), Override, Attributes_Meta_IncomingXP);

	ADD_EFFECT_MODIFIER(Modifiers, GetStrengthAttribute(), AddBase, Attributes_Primary_Strength);
	ADD_EFFECT_MODIFIER(Modifiers, GetIntelligenceAttribute(), AddBase, Attributes_Primary_Intelligence);
	ADD_EFFECT_MODIFIER(Modifiers, GetResilienceAttribute(), AddBase, Attributes_Primary_Resilience);
	ADD_EFFECT_MODIFIER(Modifiers, GetVigorAttribute(), AddBase, Attributes_Primary_Vigor);
#undef ADD_EFFECT_MODIFIER
}



UAttributesEventAbility::UAttributesEventAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
	bRetriggerInstancedAbility = true;

	const int32 Idx = AbilityTriggers.Add(FAbilityTriggerData());
	AbilityTriggers[Idx].TriggerTag = AuraGameplayTags::Attributes;
	AbilityTriggers[Idx].TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
}

void UAttributesEventAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (TriggerEventData == nullptr) return;
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(UAttributeEventEffect::StaticClass(), 1.f);
	FGameplayEffectSpec* Spec = EffectSpecHandle.Data.Get();

	for (const FGameplayModifierInfo& Mod : Spec->Def->Modifiers)
	{	// Set From IncomingXP or Default to 0.f to avoid error
		const FGameplayTag& Tag = Mod.ModifierMagnitude.GetSetByCallerFloat().DataTag;
		Spec->SetByCallerTagMagnitudes.FindOrAdd(Tag) = Tag.MatchesTagExact(TriggerEventData->EventTag) ?
			TriggerEventData->EventMagnitude : 0.f;
	}

	if (TriggerEventData->TargetData.Num() != 0)
	{
		const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0);
		if (Data->GetScriptStruct() == FGameplayAbilityTargetData_AttributeData::StaticStruct())
		{
			const FGameplayAbilityTargetData_AttributeData* AttributeData = static_cast<const FGameplayAbilityTargetData_AttributeData*>(Data);
			for (int32 i = 0; i < AttributeData->AttributeTags.Num(); i++)
			{
				Spec->SetByCallerTagMagnitudes.FindOrAdd(AttributeData->AttributeTags[i]) = AttributeData->AttributeMagnitudes[i];
			}
		}
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*Spec);
}
