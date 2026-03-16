// Copyright Hung


#include "AbilitySystem/ExecCalc/ExecCalc_Debuff.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraTag.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

UExecCalc_Debuff::UExecCalc_Debuff()
{
}
void UExecCalc_Debuff::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float Damage = 0.f;
	for (const auto& [Tag, Value] : Spec.SetByCallerTagMagnitudes)
	{
		if (AuraTag::DebuffTypeArray.Contains(Tag)) Damage += Value;
	}
	Damage *= Spec.GetStackCount();
	if (Damage > UE_KINDA_SMALL_NUMBER) AURA_ADD_OUTPUT_MODIFIER(GetIncomingDamageAttribute(), Override, Damage)
}



bool UDebuffRequirement::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
	const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const
{
	return true;
}
