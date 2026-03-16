// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_AbilityCosts.generated.h"

/**
 * Calculate and apply cost
 */
UCLASS()
class UExecCalc_AbilityCosts : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_AbilityCosts();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
