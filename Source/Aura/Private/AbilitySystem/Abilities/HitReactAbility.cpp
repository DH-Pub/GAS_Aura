// Copyright Hung


#include "AbilitySystem/Abilities/HitReactAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"

UHitReactAbility::UHitReactAbility()
{
	SetAssetTags(FGameplayTagContainer(AuraGameplayTags::Ability_HitReact));
	CancelAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Generic_Ability_Cancelable);
	BlockAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Generic_Ability_Blockable);

	/*
	 * This will add Ability_HitReact Tag to ASC, which will trigger RegisterGameplayTagEvent() instead of the following
	 * ActivateAbility: ActiveGameplayEffect = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo,
	 *		HitReactEffectClass->GetDefaultObject<UGameplayEffect>(), 1.f, 1);
	 *	EndAbility: AuraCharacterFromActorInfo->GetAuraAbilitySystemComponent()->RemoveActiveGameplayEffect(ActiveGameplayEffect, -1);
	 */
	ActivationOwnedTags.AddTag(AuraGameplayTags::Ability_HitReact); // DEPRECATED, use bMaxSpeedZeroedOnActivated

	bRetriggerInstancedAbility = true;
	
	bMaxSpeedZeroedOnActivated = true;
}