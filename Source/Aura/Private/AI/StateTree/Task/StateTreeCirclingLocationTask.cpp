// Copyright Hung

#include "AI/StateTree/Task/StateTreeCirclingLocationTask.h"

#include "NavigationSystem.h"

EStateTreeRunStatus FStateTree_CirclingLocation_Task::EnterState(FStateTreeExecutionContext& Context,
                                                                 const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this); // auto [Result, TargetActor, Radius] = Context.GetInstanceData(*this);
	if (InstanceData.TargetActor == nullptr) return EStateTreeRunStatus::Failed;
	const FVector Origin = InstanceData.TargetActor->GetActorLocation();
	FNavLocation RandomPoint(Origin);
	UWorld* World = GEngine->GetWorldFromContextObject(InstanceData.TargetActor, EGetWorldErrorMode::LogAndReturnNull);
	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
	{
		if (ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate))
		{
			if (NavSys->GetRandomPointInNavigableRadius(Origin, InstanceData.Radius, RandomPoint, UseNavData,
				UNavigationQueryFilter::GetQueryFilter(*UseNavData, InstanceData.TargetActor, nullptr)))
			{
				const auto NewPositionPtr = InstanceData.Result.GetMutablePtr<FVector>(Context);
				*NewPositionPtr = RandomPoint;
				return EStateTreeRunStatus::Succeeded;
			}
		}
	}
	
	return EStateTreeRunStatus::Failed;
}
