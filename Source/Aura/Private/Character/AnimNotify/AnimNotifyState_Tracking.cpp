// Copyright Hung


#include "Character/AnimNotify/AnimNotifyState_Tracking.h"

#include "Character/AuraCharacterBase.h"

UAnimNotifyState_Tracking::UAnimNotifyState_Tracking()
{
	bShouldFireInEditor = false;
}

void UAnimNotifyState_Tracking::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (AAuraCharacterBase* Chara = Cast<AAuraCharacterBase>(MeshComp->GetOwner()))
	{
		Chara->SetTracking(true);
	}
	Received_NotifyBegin(MeshComp, Animation, TotalDuration, EventReference); // Super::
}

void UAnimNotifyState_Tracking::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (AAuraCharacterBase* Chara = Cast<AAuraCharacterBase>(MeshComp->GetOwner()))
	{
		Chara->SetTracking(false);
	}
	Received_NotifyEnd(MeshComp, Animation, EventReference); // Super::
}
