// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Tracking.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAnimNotifyState_Tracking : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UAnimNotifyState_Tracking();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
		const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};
