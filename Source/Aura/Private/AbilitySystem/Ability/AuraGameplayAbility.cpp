// Copyright Hung


#include "AbilitySystem/Ability/AuraGameplayAbility.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"
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
	const FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	const FGameplayTagContainer& SpecTags = Spec->GetDynamicSpecSourceTags();
	if (!SpecTags.IsEmpty())
	{
		switch (ActivationPolicy)
		{
		case EAuraActivationPolicy::InputHolding:
		case EAuraActivationPolicy::InputStart:
			if (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(
				AuraGameplayTags::State_Block_Input)) return false;
			// if (Spec->InputID == 0) return false; // Invalid input set on an Input Ability
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

	if (HasAuthority(&CurrentActivationInfo))
	{

	}
	else if (IsPredictingClient())
	{
		/**
		 * This is a hack, when ability is interrupted right after client's activation
		 * Which EndAbility on Client, but Blocked on Server so Server correction for GameplayTagCountContainer never came
		 */
		FGameplayTagContainer Tags(ActivationOwnedTags);
		// if (!Tags.IsEmpty()) AuraCharacter->GetAuraAbilitySystemComponent()->ServerCheckTags(Tags);
	}
}

void UAuraGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}

void UAuraGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (EndHandle.IsValid())
	{
		if (const UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(EndHandle);
		}
	}

	if (HasAuthority(&CurrentActivationInfo))
	{

	}
	else if (IsPredictingClient())
	{
		/**
		 * This is a hack, when ability is interrupted right after client's activation
		 * Which EndAbility on Client, but Blocked on Server so Server correction for GameplayTagCountContainer never came
		 */
		const FGameplayTagContainer& OwnedTags = GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags();
		FGameplayTagContainer Tags; Tags.AppendMatchingTags(ActivationOwnedTags, OwnedTags);
		if (const FGameplayTagContainer* CDTags = GetCooldownTags())
		{
			Tags.AppendMatchingTags(*CDTags, OwnedTags);
		}
		// if (!Tags.IsEmpty()) AuraCharacter->GetAuraAbilitySystemComponent()->ServerCheckTags(Tags);
	}
}

/**
 * Epic's Comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
 * Subclasses of this call anything that requires MarkAbilitySpecDirty() before Super::
 */
void UAuraGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec); // Empty

	AuraCharacter = Cast<AAuraCharacterBase>(ActorInfo->AvatarActor);

	if (ActivationPolicy == EAuraActivationPolicy::OnSpawn)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}

	FGameplayAbilitySpec& AbilitySpec = const_cast<FGameplayAbilitySpec&>(Spec);
	if (StartupInputID != EAuraAbilityInputID::None) AbilitySpec.InputID = StartupInputID;

	if (AuraAbilityTag.IsValid()) AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbilityTag);
	ActorInfo->AbilitySystemComponent->MarkAbilitySpecDirty(AbilitySpec);
}

void UAuraGameplayAbility::DelayEndAbility(const float Time)
{
	const UWorld* World = GetWorld();
	if (!World) return;
	if (EndHandle.IsValid())
	{
		World->GetTimerManager().ClearTimer(EndHandle);
	}
	World->GetTimerManager().SetTimer(EndHandle,
		FTimerDelegate::CreateUObject(this, &UAuraGameplayAbility::K2_EndAbility), Time, false);
}
