// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraTag.h"
#include "GameplayCueManager.h"
#include "GameplayCueNotify_Actor.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Character/AuraAnimInstance.h"
#include "Character/AuraPlayer.h"
#include "Character/Component/AuraMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/AuraWidgetController.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!OnGameplayEffectAppliedDelegateToSelf.IsBoundToObject(this))
	{	// This is only called on server -> needs to convert to UFUNCTION(Client, Reliable)
		OnGameplayEffectAppliedDelegateToSelf.AddWeakLambda(this, [this](UAbilitySystemComponent*,
			const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle)
		{	// Server only
			//TODO: Add Something here
		});
	}

	if (!OnActiveGameplayEffectAddedDelegateToSelf.IsBoundToObject(this))
	{	// AddLambda will NOT add object to delegate (IsBoundToObject() == false)
		OnActiveGameplayEffectAddedDelegateToSelf.AddWeakLambda(this, [this](UAbilitySystemComponent*,
			const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle)
		{	// Will be destroyed along with ASC
			if (AbilityDataDelegate.IsBound()) // ASC is bounded to UI
			{
				FGameplayTagContainer GrantedTags; EffectSpec.GetAllGrantedTags(GrantedTags);
				if (GrantedTags.IsValid())
				{	// For Broadcasting to UI
					ABILITYLIST_SCOPE_LOCK() // Check Ability Cooldown
					for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
					{
						const FGameplayTagContainer* CooldownTags = AbilitySpec.Ability->GetCooldownTags();
						if (!CooldownTags || !CooldownTags->HasAnyExact(GrantedTags)) continue;
						UpdateAbilityData(AbilitySpec);
					}
				}
			}
		});
	}

	if (!ActiveGameplayEffects.OnActiveGameplayEffectRemovedDelegate.IsBoundToObject(this))
	{
		//TODO: Maybe bind something
	}

	if (!AbilitySpecDirtiedCallbacks.IsBoundToObject(this) && IsOwnerActorAuthoritative())
	{
		TWeakObjectPtr WeakThis = this;
		AbilitySpecDirtiedCallbacks.AddWeakLambda(this, [this, WeakThis](const FGameplayAbilitySpec&)
		{	// Use the one with delay because Client may receive data after
			if (!WeakThis.IsValid()) return;
			if (ClientRefreshTimer.IsValid()) return;
			ClientRefreshTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(
			this, [this, WeakThis]()
			{
				if (!WeakThis.IsValid()) return;
				ClientRefreshTimer.Invalidate();
				ClientRefreshAbilityData(bClearClientInput);
				bClearClientInput = false;
			}));
		});
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
				LocalInputConfirm(); // GenericLocalConfirmCallbacks is cleared in this;

				TSharedRef<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.FindOrAdd(
					FGameplayAbilitySpecHandleAndPredictionKey(AbilitySpec.Handle, OriginalPredictionKey));
				ReplicatedData->GenericEvents[1000].bTriggered = true;
				ReplicatedData->PredictionKey = OriginalPredictionKey;
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
	InputHeldHandles.Reset();
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{	// Forcefully "Release" All Inputs if Ability is not canceled
		if (!AbilitySpec.InputPressed) continue;
		AbilitySpec.InputPressed = false;
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
	for (FActiveGameplayEffect& Effect : &ActiveGameplayEffects)
	{	// ModifyActiveEffectStartTime()
		if (!Query.Matches(Effect)) continue;
		Effect.StartWorldTime -= Amount;
		Effect.StartServerWorldTime -= Amount;
		ActiveGameplayEffects.CheckDuration(Effect.Handle);
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
	ActiveEffect.EventSet.OnTimeChanged.Broadcast(ActiveEffect.Handle, ActiveEffect.StartWorldTime,
		ActiveEffect.GetDuration());
	const FGameplayTagContainer& CooldownTags = ActiveEffect.Spec.DynamicGrantedTags;
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const FGameplayTagContainer* Tags = AbilitySpec.Ability->GetCooldownTags();
		// if (Tags && Tags->HasAnyExact(CooldownTags)) ClientUpdateAbilityData(AbilitySpec.Handle);
		if (Tags && Tags->HasAnyExact(CooldownTags)) UpdateAbilityData(AbilitySpec);
	}
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
void UAuraAbilitySystemComponent::BindAbilityDataDelegateToUIDelegate(UObject* InUserObject,
	FOnReceiveAbilityDataSignature& InDelegate)
{
	AbilityDataDelegate.AddWeakLambda(InUserObject,
	[&InDelegate](const FGameplayAbilitySpec& Spec, const FAuraAbilityData& Data)
	{
		InDelegate.Broadcast(Spec, Data);
	});
}

void UAuraAbilitySystemComponent::UpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec) const
{
	if (!AbilityDataDelegate.IsBound()) return; // not bounded to UI (this may not have UI)
	if (const FAuraAbilityData* Data = UAbilityDataAsset::GetDataFromGameState(this,
		AbilitySpec.Ability.GetClass()))
	{
		AbilityDataDelegate.Broadcast(AbilitySpec, *Data);
	}
}
void UAuraAbilitySystemComponent::ClientUpdateAbilityData_Implementation(const FGameplayAbilitySpecHandle SpecHandle)
{
	if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle))
	{
		UpdateAbilityData(*Spec);
	}
}
void UAuraAbilitySystemComponent::BroadcastAllAbilityData()
{	// ASC can load before UI
	if (BroadcastDelegateTimer.IsValid()) return;
	const UWorld* World = GetWorld(); if (World == nullptr) return;
	const TWeakObjectPtr WeakThis = this;
	BroadcastDelegateTimer = World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this,
	[WeakThis, this]()
	{	// Wait for next tick so that Delegate receivers can finish loading
		if (!WeakThis.IsValid()) return;
		BroadcastDelegateTimer.Invalidate();
		ABILITYLIST_SCOPE_LOCK()
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UpdateAbilityData(AbilitySpec);
		}
	}));
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
	bClearClientInput = true;
	MarkAbilitySpecDirty(*Spec); // without MarkAbilitySpecDirty() to replicate, client Spec's won't change
}

void UAuraAbilitySystemComponent::ServerChangeAbilitySlot_Implementation(const UGameplayAbility* AbilityCDO,
	const int32 AbilityID)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(AbilityCDO->GetClass());
	if (!Spec) return;
	if (AbilityID == Spec->InputID) return; /* server check if moving to the same Slot */
	const bool bIsPassive = Cast<UAuraGameplayAbility>(Spec->Ability)->ActivationPolicy == EAuraActivationPolicy::OnSpawn;
	if (AbilityID > EAuraAbilityInputID::None)
	{
		const int32 SlotToSwap = Spec->InputID; // Store Spec's input if there is any, else EmptyTag
		if (Spec->GetDynamicSpecSourceTags().HasTag(AuraTag::Ability_Status)
			|| AbilityID < EAuraAbilityInputID::PassiveForAbilitySlots == bIsPassive /* Server side check if Ability is the same type as slot */)
		{
			ClientRefreshAbilityData(true); // tell client to refresh data to clear UI change
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
	bClearClientInput = true;
	MarkAbilitySpecDirty(*Spec);
	if (bIsPassive) HandlePassive(*Spec);
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

void UAuraAbilitySystemComponent::ClientRefreshAbilityData_Implementation(const bool bClearInput)
{
	if (bClearInput) ClearInput();
	BroadcastAllAbilityData();
}
#pragma endregion
