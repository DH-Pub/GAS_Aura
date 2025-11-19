// Copyright Hung


#include "AI/StateTree/Condition/StateTreeCombatCondition.h"
#include "StateTreeExecutionContext.h"
#include "Character/AuraEnemy.h"
#include "StateTreeNodeDescriptionHelpers.h" // UE::StateTree::DescHelpers::GetInvertText

// MUST be #undef before end of file
#define LOCTEXT_NAMESPACE "StateTreeCombatCondition"

bool FStateTreeCombatCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const auto [EnemyActor, LostTargetRange] = Context.GetInstanceData(*this);
	if (const AActor* CombatTarget = EnemyActor->CombatTarget)
	{
		const float SquaredDistance = CombatTarget->GetSquaredDistanceTo(EnemyActor);
		const bool CanEnterCombatState = SquaredDistance < LostTargetRange * LostTargetRange;
		return CanEnterCombatState ^ bInvert;
	}
	return false ^ bInvert;
}
#if WITH_EDITOR
FText FStateTreeCombatCondition::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView,
	const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);
	const FText InvertText = UE::StateTree::DescHelpers::GetInvertText(bInvert, Formatting);
	FText Range = BindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, LostTargetRange)), Formatting);

	const FText Format = (Formatting == EStateTreeNodeFormatting::RichText)
		? LOCTEXT("CombatRich", "{EmptyOrNot} is valid and in range {Range}")
		: LOCTEXT("Combat", "{EmptyOrNot} Is EmptyOrNot");
	return FText::FormatNamed(Format,
		TEXT("EmptyOrNot"), InvertText,
		TEXT("Range"), Range);
}
#endif

#undef LOCTEXT_NAMESPACE
