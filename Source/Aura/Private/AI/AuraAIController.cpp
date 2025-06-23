// Copyright Hung


#include "AI/AuraAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/StateTreeAIComponent.h"

AAuraAIController::AAuraAIController()
{
	// Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackBoard");
	// BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTree");
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>("State Tree AIComponent");
	bStartAILogicOnPossess = true; // make AI logic (StateTreeAI) start when pawn is possessed
}
