// Copyright Hung


#include "Character/AnimNotify/AnimNotify_GameplayEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UAnimNotify_GameplayEvent::UAnimNotify_GameplayEvent()
{
	bShouldFireInEditor = false;
}

void UAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{	//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
	FGameplayEventData Payload;
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		ASC->HandleGameplayEvent(EventTag, &Payload);
	}

	Super::Notify(MeshComp, Animation, EventReference);
}
