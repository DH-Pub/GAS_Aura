// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Debuff.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UExecCalc_Debuff : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_Debuff();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};


UCLASS()
class UE_DEPRECATED(5.6, "No use for this") UDebuffRequirement : public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()
public:
	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
		const FGameplayEffectSpec& Spec, UAbilitySystemComponent* ASC) const override;
};
