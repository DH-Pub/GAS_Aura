// Copyright Hung


#include "AbilitySystem/ExecCalc/ExecCalc_Debuff.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

UExecCalc_Debuff::UExecCalc_Debuff()
{
}
void UExecCalc_Debuff::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/* Boilerplate ~ */
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	/* ~ End Boilerplate */

	FDamageEffectContext& DamageContext = FAuraEffectContext::GetOrMakeContextStructRef<FDamageEffectContext>(Spec.GetContext().Get());
	// Non-Instant Effect will use the same Spec.GetContext(), so reset is needed for every execute()
	DamageContext.ResetForDebuff();
	DamageContext.TargetActor = TargetAvatar;
	if (const UDamageAbility* DamageAbility = Cast<UDamageAbility>(Spec.GetEffectContext().GetAbilityInstance_NotReplicated()))
	{
		Spec.GetContext().AddOrigin(TargetAvatar->GetActorLocation());
		const float DebuffDamage = DamageAbility->DebuffDamage.GetValueAtLevel(DamageAbility->GetAbilityLevel());
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, DebuffDamage));
	}
}



bool UDebuffRequirement::CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
	const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const
{
	return !ASC->HasMatchingGameplayTag(AuraGameplayTags::Character_State_Death);
}
