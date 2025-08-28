// Copyright Hung


#include "AI/StateTree/Condition/StateTreeCombatCondition.h"
#include "StateTreeExecutionContext.h"
#include "Character/AuraEnemy.h"
#include "StateTreeNodeDescriptionHelpers.h" // UE::StateTree::DescHelpers::GetInvertText

// MUST be #undef before end of file 
#define LOCTEXT_NAMESPACE "StateTreeCombatCondition"

bool FStateTreeCombatCondition::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const AActor* CombatTarget = InstanceData.EnemyActor->CombatTarget;
	if (!InstanceData.EnemyActor->bHitReacting && CombatTarget != nullptr)
	{
		const float SquaredDistance = CombatTarget->GetSquaredDistanceTo(InstanceData.EnemyActor);
		const bool CanEnterCombatState = SquaredDistance < InstanceData.LostTargetRange * InstanceData.LostTargetRange;
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
		? LOCTEXT("CombatRich", "{EmptyOrNot} in range {Range}")
		: LOCTEXT("Combat", "{EmptyOrNot} Is EmptyOrNot");
	return FText::FormatNamed(Format,
		TEXT("EmptyOrNot"), InvertText,
		TEXT("Range"), Range);
}
#endif

#undef LOCTEXT_NAMESPACE