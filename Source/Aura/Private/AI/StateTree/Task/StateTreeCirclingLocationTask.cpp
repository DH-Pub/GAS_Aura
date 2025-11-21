// Copyright Hung

#include "AI/StateTree/Task/StateTreeCirclingLocationTask.h"

#include "NavigationSystem.h"
#include "NavFilters/NavigationQueryFilter.h"

EStateTreeRunStatus FStateTree_CirclingLocation_Task::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	if (auto& [Result, TargetActor, Radius] = Context.GetInstanceData(*this);
		TargetActor)
	{
		const FVector Origin = TargetActor->GetActorLocation();
		UWorld* World = GEngine->GetWorldFromContextObject(TargetActor, EGetWorldErrorMode::LogAndReturnNull);
		if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
		{
			if (ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate))
			{
				FNavLocation RandomPoint;
				if (NavSys->GetRandomPointInNavigableRadius(Origin, Radius, RandomPoint, UseNavData,
					UNavigationQueryFilter::GetQueryFilter(*UseNavData, TargetActor, nullptr)))
				{
					*Result.GetMutablePtr<FVector>(Context) = RandomPoint;
					return EStateTreeRunStatus::Succeeded;
				}
			}
		}
	}
	return EStateTreeRunStatus::Failed;
}
