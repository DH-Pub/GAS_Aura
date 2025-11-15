// Copyright Hung

#pragma once

#include "StateTreeTaskBase.h"
#include "StateTreePropertyRef.h"
#include "StateTreeCirclingLocationTask.generated.h"

/**
 * 
 */
USTRUCT()
struct FStateTree_CirClingLocation_InstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=Out, meta=(RefType = "/Script/CoreUObject.Vector"))
	FStateTreePropertyRef Result; // Circling Position Result
	
	// UPROPERTY(EditAnywhere, Category=Context)
	// TObjectPtr<AActor> Actor = nullptr;
	UPROPERTY(EditAnywhere, Category=Input)
	TObjectPtr<AActor> TargetActor = nullptr;
	
	UPROPERTY(EditAnywhere, Category=Parameter)
	float Radius = 300.f;
};


USTRUCT(DisplayName="Get Circling Location", Category="Combat")
struct FStateTree_CirclingLocation_Task : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FStateTree_CirClingLocation_InstanceData;
	virtual const UStruct* GetInstanceDataType() const override {return FInstanceDataType::StaticStruct();}
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	// virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	// virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};

