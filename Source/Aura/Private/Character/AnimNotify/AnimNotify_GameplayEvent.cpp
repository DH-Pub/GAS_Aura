// Copyright Hung


#include "Character/AnimNotify/AnimNotify_GameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UAnimNotify_GameplayEvent::UAnimNotify_GameplayEvent()
{
	bShouldFireInEditor = false;
}

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
	Super::Notify(MeshComp, Animation, EventReference);
}
