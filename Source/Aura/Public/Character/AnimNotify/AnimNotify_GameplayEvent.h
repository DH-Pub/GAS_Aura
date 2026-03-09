// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_GameplayEvent.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
public:
	UAnimNotify_GameplayEvent();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
protected:
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GameplayTagFilter="GameplayEvent.Montage"))
	FGameplayTagContainer EventTags;*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(GameplayTagFilter="GameplayEvent.Montage"))
	FGameplayTag EventTag;
};
