// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraTag.h"
#include "GameplayCueManager.h"
#include "GameplayCueNotify_Actor.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"
#include "Character/AuraAnimInstance.h"
#include "Character/AuraPlayer.h"
#include "Character/Component/AuraMovementComponent.h"
#include "Player/AuraPlayerState.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!OnActiveGameplayEffectAddedDelegateToSelf.IsBoundToObject(this))
	{	// If this is not bound, that means others below aren't. All will be destroyed along with ASC
		// AddLambda will NOT add object to delegate (IsBoundToObject() == false)
		OnActiveGameplayEffectAddedDelegateToSelf.AddWeakLambda(this, [this](UAbilitySystemComponent*,
			const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle)
		{	/** Active Effect here can be Client Prediction before being removed and replaced with Server correction */
			BroadcastAllAbilityData();
		});

		/*// This is only called on server -> needs to convert to UFUNCTION(Client, Reliable)
		OnGameplayEffectAppliedDelegateToSelf.AddWeakLambda(this, [this](UAbilitySystemComponent*,
			const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle)
		{	// Server only -> we use GameplayCue instead
		});*/

		// ActiveGameplayEffects.OnActiveGameplayEffectRemovedDelegate.AddWeakLambda(this,)

		if (IsOwnerActorAuthoritative())
		{
			AbilitySpecDirtiedCallbacks.AddWeakLambda(this, [this](const FGameplayAbilitySpec&)
			{	// Use the one with delay because Client may receive data after
				BroadcastAllAbilityData();
			});
		}
	}

	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	if (UAuraAnimInstance* AnimInst = Cast<UAuraAnimInstance>(ActorInfo->GetAnimInstance()))
	{
		AnimInst->InitializeWithAbilitySystem(this);
	}

	if (UAuraMovementComponent* MovementComp = Cast<UAuraMovementComponent>(ActorInfo->MovementComponent))
	{
		MovementComp->InitializeWithAbilitySystem(this);
	}
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UAuraGameplayAbility>>& StartupActives)
{
	ABILITYLIST_SCOPE_LOCK()
	for (const TSubclassOf AbilityClass : StartupActives)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
	}
}

// ===================================== Input =====================================================================
#pragma region Activate Ability by Input =============================
void UAuraAbilitySystemComponent::AbilityInputPressed(const int32 InputID)
{
	if (GameplayTagCountContainer.HasMatchingGameplayTag(AuraTag::State_Block_Input)) return;
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.InputID != InputID) continue;
		AbilitySpec.InputPressed = true;
		const UAuraGameplayAbility* InputAbility = static_cast<UAuraGameplayAbility*>(AbilitySpec.Ability);
		if (InputAbility->ActivationPolicy == EAuraActivationPolicy::InputHolding)
		{
			InputHeldHandles.AddUnique(AbilitySpec.Handle);
		}
		else if (InputAbility->ActivationPolicy == EAuraActivationPolicy::InputStart)
		{	// We don't use bRetriggerInstancedAbility=true for input Abilities, use Input ReplicatedEvent instead
			if (AbilitySpec.IsActive()) // Use GenericReplicatedEvent, not bReplicateInputDirectly for AT_WaitInputPress
			{	// InputPressed event. Not replicated here. If someone is listening, they may replicate to the server.
				const UGameplayAbility* Instance = AbilitySpec.GetPrimaryInstance(); // Instance->InputPressed()
				const FPredictionKey OriginalPredictionKey = Instance ? /*EGameplayAbilityInstancingPolicy::InstancedPerActor*/
					Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();
				/* Send to ASC->AbilityReplicatedEventDelegate & ASC->CallReplicatedEventDelegateIfSet */
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, OriginalPredictionKey);

				//TODO: Bind to left/right mouse for confirm/cancel
				LocalInputConfirm(); // GenericLocalConfirmCallbacks is cleared in this;
			}
			else TryActivateAbility(AbilitySpec.Handle);
		}
	}
}
void UAuraAbilitySystemComponent::ProcessAbilityInput(const float DeltaTime, bool bGamePaused)
{	//can be Reset() while looping through, causing error, so we create a copy
	TArray<FGameplayAbilitySpecHandle> CopyHandles = InputHeldHandles;
	for (const FGameplayAbilitySpecHandle Handle : CopyHandles) TryActivateAbility(Handle);
}

void UAuraAbilitySystemComponent::AbilityInputReleased(const int32 InputID)
{
	if (GameplayTagCountContainer.HasMatchingGameplayTag(AuraTag::State_Block_Input)) return;
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.InputID != InputID) continue;
		AbilitySpec.InputPressed = false;
		InputHeldHandles.RemoveSwap(AbilitySpec.Handle);
		if (!AbilitySpec.IsActive()) continue;
		const UGameplayAbility* Instance = AbilitySpec.GetPrimaryInstance();
		const FPredictionKey OriginalPredictionKey = Instance ?
			Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, OriginalPredictionKey);
	}
}

void UAuraAbilitySystemComponent::ClearInput()
{
	LocalInputCancel();
	InputHeldHandles.Reset();
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{	// Forcefully "Release" All Inputs if Ability is not canceled
		if (!AbilitySpec.IsActive()) continue;
		const UGameplayAbility* Instance = AbilitySpec.GetPrimaryInstance();
		const FPredictionKey OriginalPredictionKey = Instance ?
			Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, OriginalPredictionKey);
	}
}
#pragma endregion
// ============================================================================================================


/*=============================================================================================================*/
#pragma region Abilities Functions =============================
void UAuraAbilitySystemComponent::ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, float Amount, float Percent)
{	// (Effect.GetTimeRemaining(GetWorld()->GetTimeSeconds()) - Amount) * (1 - Percent)
	if (!IsOwnerActorAuthoritative()) return; // if (!GetAvatarActor()->HasAuthority()) return;
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
	const float WorldTime = GetWorld()->GetTimeSeconds();
	for (FActiveGameplayEffect& Effect : &ActiveGameplayEffects)
	{
		if (!Query.Matches(Effect)) continue;
		if (Percent > UE_KINDA_SMALL_NUMBER)
		{
			Amount = Effect.GetTimeRemaining(WorldTime) * Percent;
		}
		Effect.StartWorldTime -= Amount; // ModifyActiveEffectStartTime()
		Effect.StartServerWorldTime -= Amount;
		ActiveGameplayEffects.CheckDuration(Effect.Handle);
		Effect.EventSet.OnTimeChanged.Broadcast(Effect.Handle, Effect.StartWorldTime, Effect.GetDuration());
		OnGameplayEffectDurationChange(Effect);
		ActiveGameplayEffects.MarkItemDirty(Effect);
		/* FGameplayEffectSpec NewSpec(Effect.Spec.Def, Effect.Spec.GetEffectContext(), Effect.Spec.GetLevel());
		NewSpec.SetDuration(FMath::Max(NewTime, .007f), true); // Cannot apply 0 duration
		NewSpec.DynamicGrantedTags = Effect.Spec.DynamicGrantedTags; // DON'T copy SetByCaller if DurMag is SetByCaller
		RemoveActiveGameplayEffect(Effect.Handle); ApplyGameplayEffectSpecToSelf(NewSpec);*/
	}
}
#pragma endregion


FActiveGameplayEffectHandle UAuraAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(
	const FGameplayEffectSpec& GameplayEffect, FPredictionKey PredictionKey)
{
	const bool bIsClientPrediction = GetOwnerRole() != ROLE_Authority && PredictionKey.IsLocalClientKey();

	const FActiveGameplayEffectHandle MyHandle = Super::ApplyGameplayEffectSpecToSelf(GameplayEffect, PredictionKey);

	if (bIsClientPrediction && MyHandle.IsValid())
	{
		/** Hack for fix: When Client Commit Ability right before being Canceled/Ended
		 * But on Server something that can block happens before Client Activation reach Server,
		 * resulting in the Ability never get Activated and Effect never gets applied on server.
		 * Client's predicted CD GE's Tags are not removed because server never applied the GE and send correction
		 */
		const FActiveGameplayEffect* ActiveEffect = GetActiveGameplayEffect(MyHandle);
		FGameplayTagContainer Tags; ActiveEffect->Spec.GetAllGrantedTags(Tags);
		if (!Tags.IsEmpty())
		{
			GetActiveEffectEventSet(MyHandle)->OnEffectRemoved.AddWeakLambda(this,
			[this](const FGameplayEffectRemovalInfo& RemovalInfo)
			{	// Will broadcast when Client's prediction is removed also
				FGameplayTagContainer InfoTags; RemovalInfo.ActiveEffect->Spec.GetAllGrantedTags(InfoTags);
				ServerCheckOwnedTags(InfoTags);
			});
		}
	}
	return MyHandle;
}


/*
 * ===========================================================================================================
 */
#pragma region Activate/Give/Remove Ability
void UAuraAbilitySystemComponent::OnGameplayEffectDurationChange(FActiveGameplayEffect& ActiveEffect)
{	// Super::OnGameplayEffectDurationChange(ActiveEffect);
	BroadcastAllAbilityData();
}
void UAuraAbilitySystemComponent::OnPredictiveGameplayCueCatchup(FGameplayTag Tag)
{
	Super::OnPredictiveGameplayCueCatchup(Tag);
}
void UAuraAbilitySystemComponent::ClientActivateAbilityFailed_Implementation(
	FGameplayAbilitySpecHandle AbilityToActivate, int16 PredictionKey)
{
	Super::ClientActivateAbilityFailed_Implementation(AbilityToActivate, PredictionKey);

}


void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	BroadcastAllAbilityData();
}
void UAuraAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{	// Super::OnTagUpdated(Tag, TagExists); // Empty
	if (TagExists)
	{
		if (Tag.MatchesTagExact(AuraTag::State_Block_Input)) ClearInput();
	}
}
void UAuraAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(AbilityActorInfo->AvatarActor))
	{	// For Player
		if (Character->IsPlayerControlled()) BroadcastAllAbilityData();
	}
}
void UAuraAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{	// Not doing anything here
	Super::OnRemoveAbility(AbilitySpec);
}
#pragma endregion


void UAuraAbilitySystemComponent::ServerCheckOwnedTags_Implementation(FGameplayTagContainer TagsToCheck)
{
	for (const FGameplayTag& Tag : GetOwnedGameplayTags())
	{
		if (TagsToCheck.HasTagExact(Tag))
		{
			TagsToCheck.RemoveTag(Tag);
		}
	}
	ClientRemoveTags(TagsToCheck);
}
void UAuraAbilitySystemComponent::ClientRemoveTags_Implementation(FGameplayTagContainer TagsToRemove)
{
	for (const FGameplayTag& Tag : TagsToRemove)
	{
		SetTagMapCount(Tag, 0);
	}
}



/*
 * ===============================================================================================================
 */
void UAuraAbilitySystemComponent::BroadcastAllAbilityData()
{	// ASC can load before UI
	if (!AbilityDataDelegate.IsBound() /* Not bound to any UI (local) */ || BroadcastDelegateTimer.IsValid()) return;
	if (const UWorld* World = GetWorld())
	{
		const TWeakObjectPtr WeakThis = this;
		BroadcastDelegateTimer = World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this,
		[WeakThis, this]()
		{	// Wait for next tick so that Delegate receivers can finish loading
			if (WeakThis.IsValid())
			{
				WeakThis->BroadcastDelegateTimer.Invalidate();
				WeakThis->AbilityDataDelegate.Broadcast();
			}
		}));
	}
}

void UAuraAbilitySystemComponent::ServerHandleGameplayEvent_Implementation(const FGameplayTag& Tag,
	const FGameplayAbilityTargetDataHandle DataHandle)
{
	FGameplayEventData Payload; Payload.TargetData = DataHandle;
	HandleGameplayEvent(Tag, &Payload); // Library::SendGameplayEventToActor()
}

#pragma region Client Upgrade
void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const UGameplayAbility* AbilityCDO)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityCDO->GetClass());
	if (!Spec) return;
	AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetOwner());
	if (!AuraPS || AuraPS->GetSpellPoints() < 1) return;
	AuraPS->AddToSpellPoints(-1);

	if (!Spec->GetDynamicSpecSourceTags().RemoveTag(AuraTag::Ability_Status_Eligible))
	{	// ability already equipable
		Spec->Level++;
	} // else consume 1 point to remove Ability_Status_Eligible
	MarkAbilitySpecDirty(*Spec); // without MarkAbilitySpecDirty() to replicate, client Spec's won't change
}

void UAuraAbilitySystemComponent::ServerChangeAbilitySlot_Implementation(const UGameplayAbility* AbilityCDO,
	const int32 AbilityID)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityCDO->GetClass());
	if (Spec && Spec->InputID != AbilityID)
	{	/* server check if moving to the same Slot */
		const bool bIsPassive = Cast<UAuraGameplayAbility>(Spec->Ability)->ActivationPolicy == EAuraActivationPolicy::OnSpawn;
		if (AbilityID > EAuraAbilityInputID::None)
		{
			const int32 SlotToSwap = Spec->InputID; // Store Spec's input if there is any, else EmptyTag
			const bool bSlotIsPassive = AbilityID > EAuraAbilityInputID::PassiveForAbilitySlots;
			if (Spec->GetDynamicSpecSourceTags().HasTag(AuraTag::Ability_Status)
				|| bSlotIsPassive != bIsPassive /* Server side check if Ability is the same type as slot */)
			{
				MarkAbilitySpecDirty(*Spec);
				// ClientRefreshAbilityData(); // tell client to refresh data to clear UI change
				return;
			}
			ABILITYLIST_SCOPE_LOCK()
			for (FGameplayAbilitySpec& OtherSpec : ActivatableAbilities.Items)
			{	// Swap out previous Ability in this slot if there is any
				if (OtherSpec.InputID != AbilityID) continue;
				OtherSpec.InputID = SlotToSwap;
				MarkAbilitySpecDirty(OtherSpec);
				if (bIsPassive) HandlePassive(OtherSpec);
			}
		}
		Spec->InputID = AbilityID; // can be 0 (UNEQUIP)
		MarkAbilitySpecDirty(*Spec);
		if (bIsPassive) HandlePassive(*Spec);
	}
}
void UAuraAbilitySystemComponent::HandlePassive(FGameplayAbilitySpec& Spec)
{
	if (Spec.InputID <= EAuraAbilityInputID::None)
	{
		CancelAbilitySpec(Spec, nullptr);
	}
	else if (Spec.InputID > EAuraAbilityInputID::PassiveForAbilitySlots)
	{
		if (!Spec.IsActive()) TryActivateAbility(Spec.Handle);
	}
}

void UAuraAbilitySystemComponent::ClientRefreshAbilityData_Implementation()
{
	BroadcastAllAbilityData();
}
#pragma endregion
