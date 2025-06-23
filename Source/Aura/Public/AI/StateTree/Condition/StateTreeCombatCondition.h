// Copyright Hung

#pragma once

#include "AITypes.h"
#include "StateTreeConditionBase.h"
#include "StateTreeAnyEnum.h"
#include "StateTreeCombatCondition.generated.h"

class AAuraEnemy;
/**
 * 
 */
USTRUCT()
struct FStateTreeCombatConditionInstanceData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category="Context")
	TObjectPtr<AAuraEnemy> EnemyActor;
	UPROPERTY(EditAnywhere, Category="Input")
	float LostTargetRange = 0.f;
};
STATETREE_POD_INSTANCEDATA(FStateTreeCombatConditionInstanceData)
USTRUCT(DisplayName="Can Enter Combat State")
struct FStateTreeCombatCondition : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FStateTreeCombatConditionInstanceData;
	virtual const UStruct* GetInstanceDataType() const override {return FInstanceDataType::StaticStruct();}
	
	FStateTreeCombatCondition() = default;
	explicit FStateTreeCombatCondition(const EStateTreeCompare InInverts)
		: bInvert(InInverts == EStateTreeCompare::Invert)
	{}

	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup,
		EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
#endif

	UPROPERTY(EditAnywhere, Category="Condition")
	bool bInvert = false;
};
