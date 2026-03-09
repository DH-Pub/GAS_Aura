// Copyright Hung


#include "AbilitySystem/AbilityTask/AT_PlayMontageWaitEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "GameFramework/Character.h"

void UAT_PlayMontageWaitEvent::OnMontageBlendingOut(UAnimMontage* Montage, const bool bInterrupted)
{
	if (Montage == MontageToPlay && Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character
			&& (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy
				&& Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{	// Reset AnimRootMotionTranslationScale
			Character->SetAnimRootMotionTranslationScale(1.f);
		}

		AbilitySystemComponent->ClearAnimatingAbility(Ability);
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (bInterrupted)
		{
			OnInterrupted.Broadcast(FGameplayTag(), FGameplayEventData());
		}
		else OnBlendOut.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UAT_PlayMontageWaitEvent::OnAbilityCancelled()
{	// TODO: Merge this fix back to engine, it was calling the wrong callback
	if (StopPlayingMontage())
	{	// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates()) OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	EndTask();
}

void UAT_PlayMontageWaitEvent::OnMontageEnded(UAnimMontage* Montage, const bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates()) OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
	}

	EndTask();
}

void UAT_PlayMontageWaitEvent::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		const_cast<FGameplayEventData*>(Payload)->EventTag = EventTag;
		// FGameplayEventData TempPayload = *Payload; TempPayload.EventTag = EventTag;
		EventReceived.Broadcast(EventTag, *Payload);
	}
}

UAT_PlayMontageWaitEvent* UAT_PlayMontageWaitEvent::PlayMontageWaitEvent(
	UAuraGameplayAbility* OwningAbility, UAnimMontage* MontageToPlay, const FGameplayTagContainer EventTags,
	float Rate, const FName StartSection, const bool bStopWhenAbilityEnds, const float AnimRootMotionTranslationScale)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UAT_PlayMontageWaitEvent* MyObj = NewAbilityTask<UAT_PlayMontageWaitEvent>(OwningAbility);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->EventTags = EventTags;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;

	return MyObj;
}

void UAT_PlayMontageWaitEvent::Activate()
{
	if (Ability == nullptr) return;

	bool bPlayedMontage = false;

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			EventHandle = ASC->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::
				FDelegate::CreateUObject(this, &UAT_PlayMontageWaitEvent::OnGameplayEvent)); // Bind event callback

			if (ASC->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection) > 0.f)
			{	// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false) return;

				CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this,
					&UAT_PlayMontageWaitEvent::OnAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UAT_PlayMontageWaitEvent::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UAT_PlayMontageWaitEvent::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character
					&& (Character->GetLocalRole() == ROLE_Authority || (Character->GetLocalRole() == ROLE_AutonomousProxy
						&& Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait call to PlayMontage failed!"));
	}
	else ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayAnimAndWait called on invalid AbilitySystemComponent"));

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait called in Ability %s failed to play montage %s; "
			"Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates()) OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}

	SetWaitingOnAvatar();
}

void UAT_PlayMontageWaitEvent::ExternalCancel()
{
	check(AbilitySystemComponent.IsValid());
	OnAbilityCancelled();

	Super::ExternalCancel();
}

void UAT_PlayMontageWaitEvent::OnDestroy(bool AbilityEnded)
{	// Note: Clearing montage end delegate isn't necessary since it's not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle); // This delegate should be cleared as it is a multicast
		if (AbilityEnded && bStopWhenAbilityEnds) StopPlayingMontage();
	}

	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}

	Super::OnDestroy(AbilityEnded);
}

bool UAT_PlayMontageWaitEvent::StopPlayingMontage()
{
	if (Ability == nullptr) return false;
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr) return false;

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr) return false;

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{	// Check if the montage is still playing
		if (ASC->GetAnimatingAbility() == Ability && ASC->GetCurrentMontage() == MontageToPlay)
		{
			if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
			{	// Unbind delegates so they don't get called as well
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}
			ASC->CurrentMontageStop(); // ability would have been interrupted, in which case we should stop the montage
			return true;
		}
	}

	return false;
}

FString UAT_PlayMontageWaitEvent::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		if (const UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance())
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? MontageToPlay : AnimInstance->GetCurrentActiveMontage();
		}
	}
	return FString::Printf(TEXT("PlayMontageAndWaitForEvent. MontageToPlay: %s  (Currently Playing): %s"),
		*GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}
