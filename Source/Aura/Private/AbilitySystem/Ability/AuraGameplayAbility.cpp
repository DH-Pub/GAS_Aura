// Copyright Hung


#include "AbilitySystem/Ability/AuraGameplayAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UAuraGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	const FGameplayTagContainer& SpecTags = Spec->GetDynamicSpecSourceTags();
	if (!SpecTags.IsEmpty())
	{
		switch (ActivationPolicy)
		{
		case EAuraActivationPolicy::InputHolding:
		case EAuraActivationPolicy::InputStart:
			if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(
				AuraGameplayTags::Character_State_Block_Input)) return false;
			if (Spec->InputID == INDEX_NONE) return false; // Invalid input set on an Input Ability
			break;
		default: break;
		}
		for (const FGameplayTag& Tag : SpecTags)
		{
			if (Tag.MatchesTagExact(AuraGameplayTags::Ability_Status_Eligible) ||
				Tag.MatchesTagExact(AuraGameplayTags::Ability_Status_Locked)) return false;
		}
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UAuraGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	// if using ActivateAbility, beware that Super::ActivateAbility calls BP_ActivateAbility inside so do it before Super
	if (bStopRotation) AuraCharacter->GetCharacterMovement()->RotationRate = FRotator();
}

void UAuraGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (bStopRotation) AuraCharacter->GetCharacterMovement()->RotationRate = AuraCharacter->BaseRotationRate;
}

// "BeginPlay" logic here, Subclasses of this call anything that requires MarkAbilitySpecDirty() before Super::
void UAuraGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{	// Super::OnAvatarSet(ActorInfo, Spec); // Parent function is empty
	AuraCharacter = Cast<AAuraCharacterBase>(ActorInfo->AvatarActor);

	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	if (ActivationPolicy == EAuraActivationPolicy::OnSpawn)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
		switch (PassiveID)
		{
		case EAuraAbilityPassiveID::None: break;
		default:
			AbilitySpec->InputID = -1 * static_cast<uint32>(PassiveID); // Negative InputID for Passives
			break;
		}
	}
	else
	{
		switch (StartupInputID)
		{
		case EAuraAbilityInputID::None: break;
		default:
			AbilitySpec->InputID = static_cast<uint32>(StartupInputID);
			break;
		}
	}

	if (AuraAbilityTag.IsValid()) AbilitySpec->GetDynamicSpecSourceTags().AddTag(AuraAbilityTag);
	ActorInfo->AbilitySystemComponent->MarkAbilitySpecDirty(*AbilitySpec);
}

FGameplayTagContainer& UAuraGameplayAbility::AddGenericAssetTags(FGameplayTagContainer& Tags)
{
	Tags.AddTag(AuraGameplayTags::Generic_Ability_Cancelable);
	// Tags.AddTag(AuraGameplayTags::Generic_Ability_Blockable);
	return Tags;
}
void UAuraGameplayAbility::SetBaseCancelBlock()
{
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Generic_Ability_Cancelable);
	// BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Generic_Ability_Blockable);
	ActivationOwnedTags.AddTag(AuraGameplayTags::Character_State_Ability);
	ActivationBlockedTags.AddTag(AuraGameplayTags::Character_State_Ability);
}

int32 UAuraGameplayAbility::ConvertInputAndPassiveEnumToAbilityID(const EAuraAbilityInputID InInputID,
	const EAuraAbilityPassiveID InPassiveID)
{
	if (InInputID != EAuraAbilityInputID::None) return static_cast<int32>(InInputID);
	if (InPassiveID != EAuraAbilityPassiveID::None) return -static_cast<int32>(InPassiveID);
	return INDEX_NONE;
}
