// Copyright Hung


#include "AbilitySystem/Ability/HitReactAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "Character/AuraCharacterBase.h"

UHitReactAbility::UHitReactAbility()
{
	CancelAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Generic_Ability_Cancelable);
	ActivationOwnedTags.AddTag(AuraGameplayTags::Character_State_HitReact);
	ActivationBlockedTags.AddTag(AuraGameplayTags::Character_State_Death);

	bRetriggerInstancedAbility = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;

	bStopRotation = bStopMovement = true;

	const int32 Idx = AbilityTriggers.Add(FAbilityTriggerData());
	AbilityTriggers[Idx].TriggerTag = AuraGameplayTags::Character_State_HitReact;
	AbilityTriggers[Idx].TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
}

void UHitReactAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (TriggerEventData == nullptr) return;
	if (const UDamageAbility* DamageAbility = Cast<UDamageAbility>(
		TriggerEventData->ContextHandle.GetAbilityInstance_NotReplicated()))
	{
		if (FMath::RandRange(0.f, 1.f) > DamageAbility->KnockbackChance) return;
		const FDamageEffectContext* DamageContext = FAuraEffectContext::GetContextStruct<FDamageEffectContext>(
			TriggerEventData->ContextHandle);
		const FVector KnockbackForce = DamageAbility->KnockbackForce * DamageContext->DamageDirection;
		AuraCharacter->LaunchCharacter(KnockbackForce, true, true);
	}
}
