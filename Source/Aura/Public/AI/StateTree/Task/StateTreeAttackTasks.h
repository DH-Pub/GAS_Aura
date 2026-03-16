// Copyright Hung

#pragma once
#include "AuraTag.h"
#include "StateTreeTaskBase.h"
#include "StateTreeAttackTasks.generated.h"

struct FGameplayTag;
class AAuraEnemy;
class AAuraAIController;

/**
 * Default Attack
 */
USTRUCT()
struct FStateTree_Attack_InstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Context)
	TObjectPtr<AAuraAIController> AIController;
	UPROPERTY(EditAnywhere, Category=Context)
	TObjectPtr<AAuraEnemy> Actor;

	UPROPERTY(EditAnywhere, Category=Parameter, meta=(GameplayTagFilter="Ability", ForceInlineRow))
	TMap<FGameplayTag, int32> AbilitiesWeight;
};
USTRUCT(DisplayName="Attack", Category="Combat")
struct FStateTree_Attack : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTree_Attack_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override {return FInstanceDataType::StaticStruct();}

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
