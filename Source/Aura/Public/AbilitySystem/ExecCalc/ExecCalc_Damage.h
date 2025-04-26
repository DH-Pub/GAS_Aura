// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * Used in GE_Damage
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
private:
	void GetAttributeMagnitude(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluateParameters,
		const FGameplayEffectAttributeCaptureDefinition& AttributeDef, float& Attribute) const;
	// FGameplayEffectAttributeCaptureDefinition SourceBlockChanceDef;
	// FGameplayEffectAttributeCaptureDefinition TargetBlockChanceDef;
};
