// Copyright Hung


#include "AI/StateTree/Task/StateTreeAttackTasks.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "StateTreeExecutionContext.h"
#include "AI/AuraAIController.h"
#include "Character/AuraEnemy.h"

EStateTreeRunStatus FStateTree_Attack::EnterState(FStateTreeExecutionContext& Context,
                                                  const FStateTreeTransitionResult& Transition) const
{
	auto [AIController, Actor, AttackTag] = Context.GetInstanceData(*this);
	AIController->StopMovement();
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
	return EStateTreeRunStatus::Succeeded;
}


#pragma region Elemetalist
EStateTreeRunStatus FStateTree_Attack_Elementalist::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	auto [AIController, Actor, AttackTag, SummonTag, MinionSpawnThreshold] = Context.GetInstanceData(*this);
	AIController->StopMovement();
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(Actor->MinionCount < MinionSpawnThreshold ? SummonTag : AttackTag));
	return EStateTreeRunStatus::Succeeded;
}
#pragma endregion