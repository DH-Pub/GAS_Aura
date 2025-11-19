// Copyright Hung


#include "AbilitySystem/Ability/HitReactAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "Character/AuraCharacterBase.h"

UHitReactAbility::UHitReactAbility()
{
	SetAssetTags(FGameplayTagContainer(AuraGameplayTags::Character_State_HitReact)); // so that Death can cancel this
	CancelAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Generic_Ability_Cancelable);
	ActivationOwnedTags.AddTag(AuraGameplayTags::Character_State_HitReact);
	ActivationOwnedTags.AddTag(AuraGameplayTags::Character_State_Block_Movement);
	ActivationBlockedTags.AddTag(AuraGameplayTags::Character_State_Death);

	bRetriggerInstancedAbility = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;

	bStopRotation = true;

	const int32 Idx = AbilityTriggers.Add(FAbilityTriggerData());
	AbilityTriggers[Idx].TriggerTag = AuraGameplayTags::Character_State_HitReact;
	AbilityTriggers[Idx].TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
}

void UHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (TriggerEventData == nullptr) return;
	if (const FDamageEffectContext* DamageContext = FAuraEffectContext::GetContextStruct<FDamageEffectContext>(
		TriggerEventData->ContextHandle))
	{
		if (!DamageContext->bKnockback) return;
		const FVector KnockbackForce = DamageContext->KnockbackForce * DamageContext->DamageDirection;
		AuraCharacter->LaunchCharacter(KnockbackForce, true, true);
		// AuraCharacter->GetCharacterMovement()->AddImpulse(KnockbackForce * 100.f);
	}
}
