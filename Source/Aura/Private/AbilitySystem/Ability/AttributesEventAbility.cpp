// Copyright Hung


#include "AbilitySystem/Ability/AttributesEventAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraTag.h"
#include "Player/AuraPlayerState.h"

UAttributesEventAbility::UAttributesEventAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
	bRetriggerInstancedAbility = true;

	FAbilityTriggerData& Data = AbilityTriggers.AddDefaulted_GetRef();
	Data.TriggerTag = AuraTag::Attributes;
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

	FGameplayEffectSpecHandle SpecHandle;
	const FGameplayTag& EventTag = TriggerEventData->EventTag;
	if (EventTag.MatchesTagExact(AuraTag::Attributes))
	{
		SpecHandle = MakeOutgoingGameplayEffectSpec(AttributeEffect, 1.f); // SetByCaller is copied here
		FGameplayEffectSpec* Spec = SpecHandle.Data.Get();
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
				EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
				return;
			}
			TotalPoints += Mag;
			Spec->SetByCallerTagMagnitudes.FindOrAdd(Tag) = Mag;
		}
		AuraPS->AddToAttributePoints(-TotalPoints);
	}
	else if (EventTag.MatchesTagExact(AuraTag::Attributes_Meta_IncomingXP))
	{
		SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
			XPEffect, 1.f, FGameplayEffectContextHandle());
		SpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(EventTag) = TriggerEventData->EventMagnitude;
	}
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}
