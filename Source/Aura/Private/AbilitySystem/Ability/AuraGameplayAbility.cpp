// Copyright Hung


#include "AbilitySystem/Ability/AuraGameplayAbility.h"

#include "AbilitySystemGlobals.h"
#include "AuraAbilityTypes.h"
#include "AuraTag.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UAuraGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!IsInstantiated())
	{
		return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	}
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	const FGameplayTagContainer& SpecTags = Spec->GetDynamicSpecSourceTags();
	if (!SpecTags.IsEmpty())
	{
		switch (ActivationPolicy)
		{
		case EAuraActivationPolicy::InputHolding:
		case EAuraActivationPolicy::InputStart:
			if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(
				AuraTag::State_Block_Input)) return false;
			// if (Spec->InputID == 0) return false; // Invalid input set on an Input Ability
			break;
		case EAuraActivationPolicy::OnSpawn:
			if (Spec->InputID < EAuraAbilityInputID::PassiveForAbilitySlots)
			{	//TODO: Add/Remove Ability instead, save upgrade data somewhere
				return false;
			}
			break;
		}
		for (const FGameplayTag& Tag : SpecTags)
		{
			if (Tag.MatchesTagExact(AuraTag::Ability_Status_Eligible) ||
				Tag.MatchesTagExact(AuraTag::Ability_Status_Locked)) return false;
		}
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

/**
 * Called in OnGiveAbility();
 * Epic's Comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
 * Subclasses of this call anything that requires MarkAbilitySpecDirty() before Super::
 */
void UAuraGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec); // Empty
	BP_OnAvatarSet();

	AuraCharacter = Cast<AAuraCharacterBase>(ActorInfo->AvatarActor);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (ActivationPolicy == EAuraActivationPolicy::OnSpawn)
	{
		ASC->TryActivateAbility(Spec.Handle);
	}

	FGameplayAbilitySpec& AbilitySpec = const_cast<FGameplayAbilitySpec&>(Spec);
	if (StartupInputID > EAuraAbilityInputID::None) AbilitySpec.InputID = StartupInputID;

	ASC->MarkAbilitySpecDirty(AbilitySpec);
}

void UAuraGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec); // Empty
	BP_OnRemoveAbility();
}

const FAuraAbilityActorInfo* UAuraGameplayAbility::GetAuraActorInfo() const
{
	return static_cast<const FAuraAbilityActorInfo*>(CurrentActorInfo);
}

UAuraAbilitySystemComponent* UAuraGameplayAbility::GetAuraASC() const
{
	if (!ensure(CurrentActorInfo)) return nullptr;
	return static_cast<UAuraAbilitySystemComponent*>(CurrentActorInfo->AbilitySystemComponent.Get());
}
