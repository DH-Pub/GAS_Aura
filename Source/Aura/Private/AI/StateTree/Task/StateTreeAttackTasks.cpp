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
	auto& [AIController, Actor, AbilitiesWeight] =
		Context.GetInstanceData(*this);
	AIController->StopMovement();

	int32 TotalWeight = 0;
	for (const auto [Tag, Weight] : AbilitiesWeight) TotalWeight += Weight;
	int32 RandWeight = FMath::RandRange(0, TotalWeight);
	FGameplayTag WeightTag = FGameplayTag::EmptyTag;
	TArray<FGameplayTag> OtherTags; AbilitiesWeight.GetKeys(OtherTags);
	for (const auto& [Tag, Weight] : AbilitiesWeight)
	{
		if (RandWeight <= Weight)
		{
			WeightTag = Tag;
			break;
		}
		RandWeight -= Weight;
	}
	OtherTags.RemoveSingleSwap(WeightTag);

	UAuraAbilitySystemComponent* ASC = UAuraAbilitySystemGlobals::GetAuraASC(Actor);
	if (!ASC->TryActivateAbilityByDynamicTag(WeightTag))
	{
		const int32 LastIdx = OtherTags.Num() - 1;
		for (int32 i = 0; i < LastIdx; ++i)
		{	// Shuffle
			OtherTags.Swap(i, FMath::RandRange(i, LastIdx));
		}
		for (const FGameplayTag& Tag : OtherTags)
		{
			if (ASC->TryActivateAbilityByDynamicTag(Tag)) break;
		}
	}
	return EStateTreeRunStatus::Succeeded;
}
