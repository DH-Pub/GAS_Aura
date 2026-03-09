// Copyright Hung


#include "AbilitySystem/Ability/AttributesEventAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

UAttributesEventAbility::UAttributesEventAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	bRetriggerInstancedAbility = true;

	FAbilityTriggerData& Data = AbilityTriggers.AddDefaulted_GetRef();
	Data.TriggerTag = AuraGameplayTags::Attributes;
	Data.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
}

void UAttributesEventAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec); // Put anything that requires MarkAbilitySpecDirty() before this

	AuraPS = Cast<AAuraPlayerState>(ActorInfo->AbilitySystemComponent->GetOwner());

	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	for (const FGameplayModifierInfo& Mod : AttributeEffect->GetDefaultObject<UGameplayEffect>()->Modifiers)
	{	// Set From IncomingXP or Default to 0.f to avoid error
		const FGameplayTag& Tag = Mod.ModifierMagnitude.GetSetByCallerFloat().DataTag;
		AbilitySpec->SetByCallerTagMagnitudes.FindOrAdd(Tag) = 0.f;
	}
}

void UAttributesEventAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!HasAuthority(&ActivationInfo) || !TriggerEventData || !AuraPS) return;
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttributeEffect, 1.f);
	FGameplayEffectSpec* Spec = EffectSpecHandle.Data.Get();
	Spec->SetByCallerTagMagnitudes = GetCurrentAbilitySpec()->SetByCallerTagMagnitudes;

	const FGameplayTag& EventTag = TriggerEventData->EventTag;
	if (EventTag.MatchesTagExact(AuraGameplayTags::Attributes))
	{
		const FGameplayAbilityTargetData* Data = TriggerEventData->TargetData.Get(0); ensure(Data);
		if (AuraPS && Data->GetScriptStruct() != FGameplayAbilityTargetData_AttributeData::StaticStruct()) return;
		const FGameplayAbilityTargetData_AttributeData* AttributeData =
			static_cast<const FGameplayAbilityTargetData_AttributeData*>(Data);
		int32 TotalPoints = 0;
		for (auto& [Tag, Mag] : AttributeData->Data)
		{
			if (Mag < 0)
			{	// Does not allow Client to send -Mag
				AuraPS->AddToAttributePoints(0); // To reset UI on Client
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
				return;
			}
			TotalPoints += Mag;
			Spec->SetSetByCallerMagnitude(Tag, Mag);
		}
		AuraPS->AddToAttributePoints(-TotalPoints);
	}
	else if (EventTag.MatchesTagExact(AuraGameplayTags::Attributes_Meta_IncomingXP))
	{
		Spec->SetSetByCallerMagnitude(EventTag, TriggerEventData->EventMagnitude);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*Spec);
}


// ====================== GE_Attribute ==============================================
UAttributeEventEffect::UAttributeEventEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

#define ADD_EFFECT_MODIFIER(Property, Op, Tag)\
{\
	FGameplayModifierInfo& Info = Modifiers.AddDefaulted_GetRef();\
	Info.Attribute = UAuraAttributeSet::Property; Info.ModifierOp = EGameplayModOp::Op;\
	FSetByCallerFloat SetByCallerFloat; SetByCallerFloat.DataTag = AuraGameplayTags::Tag;\
	Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCallerFloat);\
}

	ADD_EFFECT_MODIFIER(GetStrengthAttribute(), AddBase, Attributes_Primary_Strength);
	ADD_EFFECT_MODIFIER(GetIntelligenceAttribute(), AddBase, Attributes_Primary_Intelligence);
	ADD_EFFECT_MODIFIER(GetResilienceAttribute(), AddBase, Attributes_Primary_Resilience);
	ADD_EFFECT_MODIFIER(GetVigorAttribute(), AddBase, Attributes_Primary_Vigor);
	ADD_EFFECT_MODIFIER(GetIncomingXPAttribute(), Override, Attributes_Meta_IncomingXP);

#undef ADD_EFFECT_MODIFIER
}
