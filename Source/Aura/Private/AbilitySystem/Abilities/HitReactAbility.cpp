// Copyright Hung


#include "AbilitySystem/Abilities/HitReactAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"

UHitReactAbility::UHitReactAbility()
{
	SetAssetTags(FGameplayTagContainer(AuraGameplayTags::Ability_HitReact));
	CancelAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Ability_Type_Activatable_Blockable);
	BlockAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Ability_Type_Activatable_Blockable);

	bRetriggerInstancedAbility = true;
}

void UHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ActiveGameplayEffect = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo,
		HitReactEffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, 1);
}

void UHitReactAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (HasAuthority(&CurrentActivationInfo) == false) return;
	AuraCharacterFromActorInfo->GetAuraAbilitySystemComponent()->RemoveActiveGameplayEffect(ActiveGameplayEffect, -1);
}
