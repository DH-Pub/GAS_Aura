// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Character/AuraPlayer.h"
#include "Player/AuraPlayerState.h"

void UAuraAbilitySystemComponent::InitAuraASC(AActor* InOwnerActor, AAuraCharacterBase* AuraCharacter)
{
	InitAbilityActorInfo(InOwnerActor, AuraCharacter);
	if (!OnGameplayEffectAppliedDelegateToSelf.IsBound())
	{	// This is only called on server -> needs to convert to UFUNCTION(Client, Reliable)
		OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
	}
	AuraCharacter->FinishedAbilitySystemCompInit(this);
}
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer; EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer); // Broadcast to OverlayWidgetController.cpp ASC->EffectAssetTags.AddLambda()
}


void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UAuraGameplayAbility>>& StartupActives)
{
	for (const TSubclassOf AbilityClass : StartupActives) GiveAbility(FGameplayAbilitySpec(AbilityClass, 1));
}

// ===================================== Input =====================================================================
#pragma region Activate Ability by Input =============================
void UAuraAbilitySystemComponent::AbilityInputPressed(const int8 InputID)
{
	if (GameplayTagCountContainer.HasMatchingGameplayTag(AuraGameplayTags::Character_State_Block_Input)) return;

	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.InputID != InputID) continue;
		const UAuraGameplayAbility* InputAbility = static_cast<UAuraGameplayAbility*>(AbilitySpec.Ability);
		if (InputAbility->ActivationPolicy == EAuraActivationPolicy::InputHolding)
		{
			InputHeldHandles.AddUnique(AbilitySpec.Handle);
		}
		else if (InputAbility->ActivationPolicy == EAuraActivationPolicy::InputStart)
		{	// We don't use bRetriggerInstancedAbility = true bc Input is converted to ReplicatedEvent if IsActive()
			if (AbilitySpec.IsActive()) // Use GenericReplicatedEvent, not bReplicateInputDirectly for AT_WaitInputPress
			{	// InputPressed event. Not replicated here. If someone is listening, they may replicate to the server.
				const UGameplayAbility* Instance = AbilitySpec.GetPrimaryInstance(); // Instance->InputPressed()
				const FPredictionKey OriginalPredictionKey = Instance ?
					Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();
				/* Send to ASC->AbilityReplicatedEventDelegate & ASC->CallReplicatedEventDelegateIfSet */
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, OriginalPredictionKey);
			}
			else TryActivateAbility(AbilitySpec.Handle);
		}
	}
}
void UAuraAbilitySystemComponent::ProcessAbilityInput(const float DeltaTime, bool bGamePaused)
{
	for (const FGameplayAbilitySpecHandle SpecHandle : InputHeldHandles) TryActivateAbility(SpecHandle);
}

void UAuraAbilitySystemComponent::AbilityInputReleased(const int8 InputID)
{
	if (GameplayTagCountContainer.HasMatchingGameplayTag(AuraGameplayTags::Character_State_Block_Input)) return;

	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.InputID != InputID) continue;
		InputHeldHandles.Remove(AbilitySpec.Handle);
		if (!AbilitySpec.IsActive()) continue;
		const UGameplayAbility* Instance = AbilitySpec.GetPrimaryInstance();
		const FPredictionKey OriginalPredictionKey = Instance ?
			Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, OriginalPredictionKey);
	}
}
#pragma endregion
// ============================================================================================================


FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	ABILITYLIST_SCOPE_LOCK()
	for (FGameplayAbilitySpec& ActivatableSpec : ActivatableAbilities.Items)
	{
		if (ActivatableSpec.GetDynamicSpecSourceTags().HasTagExact(AbilityTag)) return &ActivatableSpec;
	}
	return nullptr;
}

/*=============================================================================================================*/
#pragma region Abilities Functions =============================
void UAuraAbilitySystemComponent::ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, const float Amount, float Percent)
{
	if (!IsOwnerActorAuthoritative()) return; // if (!GetAvatarActor()->HasAuthority()) return;
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
	for (const FActiveGameplayEffectHandle ActiveHandle : ActiveGameplayEffects.GetActiveEffects(Query))
	{
		FActiveGameplayEffect* ActiveEffect = ActiveGameplayEffects.GetActiveGameplayEffect(ActiveHandle);
		const float TimeRemaining = ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		constexpr float MinFrame = .007f;
		float NewTime = FMath::Max(TimeRemaining - Amount, MinFrame); // Cannot apply effect with 0 duration
		if (Percent > UE_KINDA_SMALL_NUMBER)
		{
			Percent = FMath::Min(Percent, 0.6); /*Max CD 60%*/
			NewTime = FMath::Max(NewTime * (1 - Percent), MinFrame);
		}

		FGameplayEffectSpec& OldSpec = ActiveEffect->Spec;
		FGameplayEffectSpec Spec(OldSpec.Def, OldSpec.GetEffectContext(), OldSpec.GetLevel());
		Spec.SetDuration(NewTime, true);
		Spec.DynamicGrantedTags = OldSpec.DynamicGrantedTags; // DON'T copy SetByCaller if DurMag is SetByCaller

		const float* OriginCD = OldSpec.SetByCallerTagMagnitudes.Find(AuraGameplayTags::Ability_Cooldown_Duration);
		Spec.SetByCallerTagMagnitudes.Add(AuraGameplayTags::Ability_Cooldown_Duration) =
			OriginCD ? *OriginCD : OldSpec.GetDuration();

		RemoveActiveGameplayEffect(ActiveEffect->Handle);
		ApplyGameplayEffectSpecToSelf(Spec);
	}
}
#pragma endregion


/*
 * ===========================================================================================================
 */
#pragma region Activate/Give/Remove Ability
void UAuraAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{	// Super::OnTagUpdated(Tag, TagExists); // Parent function is empty and only for override
	if (Tag.MatchesTagExact(AuraGameplayTags::Character_State_Block_Input))
	{
		if (GameplayTagCountContainer.HasMatchingGameplayTag(AuraGameplayTags::Character_State_Block_Input))
		{
			InputHeldHandles.Reset();
		}
	}
}
void UAuraAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(AbilityActorInfo->AvatarActor))
	{
		if (Character->IsPlayerControlled()) ClientUpdateAbilityData(AbilitySpec);
	}
}
void UAuraAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(AbilityActorInfo->AvatarActor))
	{
		if (Character->IsPlayerControlled())
		{
			//TODO: Add Remove Broadcast
		}
	}
}
#pragma endregion



/*
 * ===============================================================================================================
*/
void UAuraAbilitySystemComponent::ClientUpdateAbilityData_Implementation(const FGameplayAbilitySpec& AbilitySpec)
{
	UpdateAbilityData(AbilitySpec);
}
void UAuraAbilitySystemComponent::UpdateAbilityData(const FGameplayAbilitySpec& AbilitySpec) const
{
	if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(this,
		AbilitySpec.GetDynamicSpecSourceTags()))
	{
		FPlayerAbilityData PlayerData;
		PlayerData.AbilityID = AbilitySpec.InputID;
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(AuraGameplayTags::Ability_Status)) PlayerData.StatusTag = Tag;
		}
		AbilityDataDelegate.Broadcast(*Data, PlayerData);
	}
}
void UAuraAbilitySystemComponent::BroadcastAllAbilityData()
{
	const UWorld* World = GetWorld();
	if (World == nullptr || BroadcastDelegate.IsValid()) return;
	if (BroadcastDelegate.IsValid()) World->GetTimerManager().ClearTimer(BroadcastDelegate);
	BroadcastDelegate = World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]()
	{	// Wait for next tick so that Delegate receivers can finish loading
		BroadcastDelegate.Invalidate();
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			UpdateAbilityData(AbilitySpec);
		}
	}));
}

#pragma region Client Upgrade
void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const TArray<FPointAllocation>& PointsAllocated)
{
	if (AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetOwner()))
	{
		FGameplayEventData Payload;
		FGameplayAbilityTargetData_AttributeData* Data = new FGameplayAbilityTargetData_AttributeData();
		for (const auto& [AttributeTag, AddedPoints] : PointsAllocated)
		{
			if (AddedPoints > AuraPS->GetAttributePoints() || AddedPoints < 1) break;
			Data->AttributeTags.Add(AttributeTag);
			Data->AttributeMagnitudes.Add(AddedPoints);
			AuraPS->AddToAttributePoints(-AddedPoints);
			/*Payload.EventTag = AttributeTag; Payload.EventMagnitude = AddedPoints;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), Payload.EventTag, Payload);*/
		}
		Payload.TargetData = FGameplayAbilityTargetDataHandle(Data);
		HandleGameplayEvent(AuraGameplayTags::Attributes, &Payload);
		ClientFinishUpgradeAttribute();
	}
}
void UAuraAbilitySystemComponent::ClientFinishUpgradeAttribute_Implementation()
{
	OnApplyingStatFinishedDelegate.Broadcast();
}


void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetOwner()))
		{
			if (AuraPS->GetSpellPoints() < 1) return;
			AuraPS->AddToSpellPoints(-1);

			if (!Spec->GetDynamicSpecSourceTags().RemoveTag(AuraGameplayTags::Ability_Status_Eligible))
			{	// ability already equipable
				Spec->Level++; // without MarkAbilitySpecDirty() to replicate, client Spec's won't change
			} // else consume 1 point to remove Ability_Status_Eligible
			MarkAbilitySpecDirty(*Spec);
			ClientUpdateAbilityData(*Spec);
		}
	}
}

void UAuraAbilitySystemComponent::ServerChangeAbilitySlot_Implementation(const FGameplayTag& AbilityTag, const int32 AbilityID)
{
	FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag);
	if (Spec == nullptr) return;
	if (AbilityID == Spec->InputID) return; /* server check if moving to the same Slot */
	if (AbilityID != INDEX_NONE)
	{
		const int32 SlotToSwap = Spec->InputID; // Store Spec's input if there is any, else EmptyTag
		const bool bIsPassive = Cast<UAuraGameplayAbility>(Spec->Ability)->ActivationPolicy == EAuraActivationPolicy::OnSpawn;
		if (Spec->GetDynamicSpecSourceTags().HasTag(AuraGameplayTags::Ability_Status)
			|| AbilityID < -1 != bIsPassive /* Server side check if Ability is the same type as slot */)
		{
			ClientRefreshAbilityData(); // tell client to refresh data to clear UI change
			return;
		}
		ABILITYLIST_SCOPE_LOCK()
		for (FGameplayAbilitySpec& OtherSpec : ActivatableAbilities.Items)
		{	// Swap out previous Ability in this slot if there is any
			if (OtherSpec.InputID == AbilityID)
			{
				OtherSpec.InputID = SlotToSwap;
				MarkAbilitySpecDirty(OtherSpec);
			}
		}
	}
	Spec->InputID = AbilityID; // can be INDEX_NONE (UNEQUIP)
	MarkAbilitySpecDirty(*Spec);
	ClientRefreshAbilityData();
}
void UAuraAbilitySystemComponent::ClientRefreshAbilityData_Implementation()
{
	InputHeldHandles.Reset();
	BroadcastAllAbilityData();
}
#pragma endregion


/*
 * TryActivateAbilitiesByTag only use AssetTags, this will check GetDynamicSpecSourceTags()
 */
bool UAuraAbilitySystemComponent::TryActivateAbilityByDynamicTag(const FGameplayTag& Tag,
	const bool bAllowRemoteActivation)
{
	if (!Tag.IsValid()) return false;
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{	// GetActivatableGameplayAbilitySpecsByAllMatchingTags
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTag(Tag))
		{
			if (TryActivateAbility(Spec.Handle, bAllowRemoteActivation)) return true;
		}
	}
	return false;
}

void UAuraAbilitySystemComponent::NetMulticast_InvokeGameplayCueExecuted_WithParams_Implementation(
	const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters)
{
	if (!IsOwnerActorAuthoritative() && PredictionKey.IsLocalClientKey()) return;

	InvokeGameplayCueEvent(GameplayCueTag, EGameplayCueEvent::Executed, GameplayCueParameters);
	FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(GameplayCueParameters.EffectContext);
	if (AuraContext == nullptr) return;
	FGameplayCueParameters CueParameters(GameplayCueParameters.EffectContext);
	for (const FEffectCues& EffectCue : AuraContext->GetEffectCuesList())
	{
		CueParameters.RawMagnitude = EffectCue.RawMagnitude;
		InvokeGameplayCueEvent(EffectCue.CueTag, EGameplayCueEvent::Executed, CueParameters);
	}

	for (FCoreCueParams& Cue : AuraContext->GetCueParamsBatched())
	{
		Cue.UnpackAndInvokeGameplayCueEvent(this);

		FGameplayCueParameters Params(Cue.EffectContext);
		FAuraEffectContext* Context = FAuraEffectContext::ExtractAuraContext(Cue.EffectContext);
		for (const FEffectCues& EffectCue : Context->GetEffectCuesList())
		{
			Params.RawMagnitude = EffectCue.RawMagnitude;
			InvokeGameplayCueEvent(EffectCue.CueTag, EGameplayCueEvent::Executed, Params);
		}
	}
}
