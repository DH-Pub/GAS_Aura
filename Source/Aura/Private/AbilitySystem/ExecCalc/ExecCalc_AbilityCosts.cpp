// Copyright Hung


#include "AbilitySystem/ExecCalc/ExecCalc_AbilityCosts.h"

#include "AuraTag.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"

/*struct FAuraCostStatics
{
	AURA_ATTR_DEFINE_DEF(TargetIntelligenceDef, GetIntelligenceAttribute(), Source, false)
	const TArray<FGameplayEffectAttributeCaptureDefinition> DefArray = {TargetIntelligenceDef,};
};
static const FAuraCostStatics& CostStatics() {static FAuraCostStatics S; return S;}*/

UExecCalc_AbilityCosts::UExecCalc_AbilityCosts()
{
	// RelevantAttributesToCapture.Append(CostStatics().DefArray);
}
void UExecCalc_AbilityCosts::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	/*FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	float Intelligence;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CostStatics().TargetIntelligenceDef, EvaluateParameters,
		Intelligence);*/

	if (const UCostCooldownAbility* Ability = Cast<UCostCooldownAbility>(
		Spec.GetEffectContext().GetAbilityInstance_NotReplicated()))
	{
		FGameplayModifierEvaluatedData Data; Data.ModifierOp = EGameplayModOp::AddBase;
		for (auto& [Tag, ScalableCost] : Ability->AbilityCosts)
		{
			Data.Magnitude = -ScalableCost.GetValueAtLevel(Spec.GetLevel());
			if (Tag.MatchesTagExact(AuraTag::Attributes_Vital_Mana))
			{
				Data.Attribute = UAuraAttributeSet::GetManaAttribute();
			}
			else if (Tag.MatchesTagExact(AuraTag::Attributes_Vital_Health))
			{
				Data.Attribute = UAuraAttributeSet::GetHealthAttribute();
			}
			OutExecutionOutput.AddOutputModifier(Data);
		}
	}
}
