// Copyright Hung


#include "AI/StateTree/Task/StateTreeAttackTasks.h"

#include "AuraAbilitySystemGlobals.h"
#include "StateTreeExecutionContext.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AI/AuraAIController.h"
#include "Character/AuraEnemy.h"

EStateTreeRunStatus FStateTree_Attack::EnterState(FStateTreeExecutionContext& Context,
                                                  const FStateTreeTransitionResult& Transition) const
{
	auto& [AIController, Actor, AttackTag] = Context.GetInstanceData(*this);
	AIController->StopMovement();
	UAuraAbilitySystemComponent* ASC = UAuraAbilitySystemGlobals::GetAuraASC(Actor);
	ASC->TryActivateAbilityByDynamicTag(AttackTag);
	return EStateTreeRunStatus::Succeeded;
}


#pragma region Elemetalist
EStateTreeRunStatus FStateTree_Attack_Elementalist::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	auto& [AIController, Actor, AttackTag, SummonTag]
		= Context.GetInstanceData(*this);
	AIController->StopMovement();
	UAuraAbilitySystemComponent* ASC = UAuraAbilitySystemGlobals::GetAuraASC(Actor);
	if (!ASC->TryActivateAbilityByDynamicTag(SummonTag)) // if Summon CanActivateAbility returns false
	{
		ASC->TryActivateAbilityByDynamicTag(AttackTag);
	}
	return EStateTreeRunStatus::Succeeded;
}
#pragma endregion