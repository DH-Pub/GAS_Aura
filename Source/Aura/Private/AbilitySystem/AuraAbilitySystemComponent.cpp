// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Character/AuraPlayer.h"
#include "Player/AuraPlayerState.h"
#include "GameplayCueManager.h"

void UAuraAbilitySystemComponent::InitAuraASC(AActor* InOwnerActor, AAuraCharacterBase* AuraCharacter)
{
	InitAbilityActorInfo(InOwnerActor, AuraCharacter);
	if (!OnGameplayEffectAppliedDelegateToSelf.IsBound())
	{	// This is only called on server -> needs to convert to UFUNCTION(Client, Reliable)
		OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
	}
	AuraCharacter->GetDebuffNiagaraComponent()->SetASC(this);
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

void UAuraAbilitySystemComponent::UnlockAbilityByLevel(const int32 CharacterLevel)
{
	for (const FAuraAbilityData& Data : UAbilityDataAsset::GetFromGameState(this)->AbilityDataList)
	{	 /* not enough lv or already has ability */
		if (CharacterLevel < Data.LevelRequirement || GetSpecFromAbilityTag(Data.GetAuraAbilityTag())) continue;
		FGameplayAbilitySpec AbilitySpec(Data.AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Ability_Status_Eligible);
		GiveAbility(AbilitySpec);
	}
}

// ===================================== Input =====================================================================
#pragma region Activate Ability by Input =============================
void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;
		const UAuraGameplayAbility* InputAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability);
		if (AbilitySpec.InputPressed)
		{	// Already pressed and is now holding
			if (InputAbility->ActivationPolicy == EAuraActivationPolicy::InputHolding)
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
			continue;
		} // else start pressed
		AbilitySpec.InputPressed = true; // AbilitySpecInputPressed(AbilitySpec);
		if (AbilitySpec.IsActive())
		{	// Use Generic Replicated Events instead of bReplicateInputDirectly for WaitInputPress AbilityTask to works.
			const UGameplayAbility* Instance = AbilitySpec.GetPrimaryInstance(); // Instance->InputPressed()
			const FPredictionKey OriginalPredictionKey = Instance ?
				Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : FPredictionKey();
			// InputPressed event. Not replicated here. If someone is listening, they may replicate the event to the server.
			/* Send to ASC->AbilityReplicatedEventDelegate & ASC->CallReplicatedEventDelegateIfSet */
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, OriginalPredictionKey);
		}
		else TryActivateAbility(AbilitySpec.Handle);
	}
}
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;
		AbilitySpec.InputPressed = false; // AbilitySpecInputReleased(AbilitySpec);
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
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& ActivatableSpec : ActivatableAbilities.Items)
	{
		if (ActivatableSpec.GetDynamicSpecSourceTags().HasTagExact(AbilityTag)) return &ActivatableSpec;
	}
	return nullptr;
}
const FGameplayTag& UAuraAbilitySystemComponent::GetInputFromSpec(const FGameplayAbilitySpec* Spec)
{
	for (const FGameplayTag& Tag : Spec->GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(AuraGameplayTags::Input)) return Tag;
	}
	return FGameplayTag::EmptyTag;
}

/*=============================================================================================================*/
#pragma region Abilities Functions =============================
void UAuraAbilitySystemComponent::ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, const float Amount, float Percent)
{
	if (!GetAvatarActor()->HasAuthority()) return;
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
	for (const FActiveGameplayEffectHandle& ActiveHandle : ActiveGameplayEffects.GetActiveEffects(Query))
	{
		FActiveGameplayEffect* ActiveEffect = ActiveGameplayEffects.GetActiveGameplayEffect(ActiveHandle);
		const float TimeRemaining = ActiveEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
		constexpr float MinFrame = .007f;
		float NewTime = FMath::Max(TimeRemaining - Amount, MinFrame); // Cannot apply effect with 0 duration
		if (Percent > UE_KINDA_SMALL_NUMBER)
		{
			Percent = FMath::Min(Percent, 0.6); // Max Cooldown 60%
			NewTime = FMath::Max(NewTime * (1 - Percent), MinFrame);
		}

		FGameplayEffectSpec& OldSpec = ActiveEffect->Spec;
		FGameplayEffectSpec Spec(OldSpec.Def, OldSpec.GetEffectContext(), OldSpec.GetLevel());
		Spec.SetDuration(NewTime, true);
		Spec.DynamicGrantedTags = OldSpec.DynamicGrantedTags; // DO NOT copy SetByCallerTagMagnitudes

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
{
	Super::OnTagUpdated(Tag, TagExists);
	//TODO: Find something to do with this
}
void UAuraAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor()))
	{
		if (Character->IsPlayerControlled())
		{
			ClientUpdateAbilityData(AbilitySpec);
		}
	}
}
void UAuraAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor()))
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
void UAuraAbilitySystemComponent::ClientUpdateAbilityData_Implementation(const FGameplayAbilitySpec& AbilitySpec) const
{
	if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(this,
		AbilitySpec.GetDynamicSpecSourceTags()))
	{
		FPlayerAbilityData PlayerData;
		for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTag(AuraGameplayTags::Input)) {PlayerData.InputTag = Tag;}
			else if (Tag.MatchesTag(AuraGameplayTags::Ability_Status)) {PlayerData.StatusTag = Tag;}
		}
		AbilityDataDelegate.Broadcast(*Data, PlayerData);
	}
}

#pragma region Client Upgrade
void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const TArray<FPointAllocation>& PointsAllocated)
{
	AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetOwner());
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
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AuraGameplayTags::Attributes, Payload);
	ClientFinishUpgradeAttribute();
}
void UAuraAbilitySystemComponent::ClientFinishUpgradeAttribute_Implementation()
{
	OnApplyingStatFinishedDelegate.Broadcast();
}


void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	AAuraPlayerState* AuraPS = Cast<AAuraPlayerState>(GetOwner());
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (AuraPS->GetSpellPoints() < 1) return;
		AuraPS->AddToSpellPoints(-1);

		if (Spec->GetDynamicSpecSourceTags().RemoveTag(AuraGameplayTags::Ability_Status_Eligible))
		{	// need to spend 1 point to unlock
			Spec->GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Ability_Status_Unlocked);
		}
		else if (Spec->GetDynamicSpecSourceTags().HasTagExact(AuraGameplayTags::Ability_Status_Unlocked))
		{
			Spec->Level++; // without MarkAbilitySpecDirty(), client Spec's won't change
		}
		MarkAbilitySpecDirty(*Spec);
		ClientUpdateAbilityData(*Spec);
	}
}
// Change to SlotTag
void UAuraAbilitySystemComponent::ServerChangeAbilitySlot_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& SlotTag)
{
	FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag);
	if (Spec == nullptr || !Spec->GetDynamicSpecSourceTags().HasTagExact(AuraGameplayTags::Ability_Status_Unlocked)) return;
	const FGameplayTag& SlotToSwap = GetInputFromSpec(Spec); // Store Spec's input if there is any
	if (SlotToSwap.MatchesTagExact(SlotTag)) return; // if Ability is moved to the same Slot
	if (SlotTag.MatchesTag(AuraGameplayTags::Input_Combat_Passive) /*Server side check valid type*/
		!= (Cast<UAuraGameplayAbility>(Spec->Ability)->ActivationPolicy == EAuraActivationPolicy::OnSpawn)) return;
	if (SlotTag.IsValid()) // if not, Slot will just be removed
	{
		FScopedAbilityListLock AbilityListLock(*this);
		for (FGameplayAbilitySpec& OtherSpec : ActivatableAbilities.Items)
		{	// Swap out previous Ability in this SlotTag if there is any
			if (OtherSpec.GetDynamicSpecSourceTags().RemoveTag(SlotTag))
			{
				if (SlotToSwap.IsValid()) OtherSpec.GetDynamicSpecSourceTags().AddTag(SlotToSwap);
				MarkAbilitySpecDirty(OtherSpec);
				ClientUpdateAbilityData(OtherSpec);
			}
		}
		Spec->GetDynamicSpecSourceTags().AddTag(SlotTag); // Can Assign to SlotTag after loop clear all SlotTag
	}
	Spec->GetDynamicSpecSourceTags().RemoveTag(SlotToSwap); // Remove Input and use SlotToSwap from here on
	MarkAbilitySpecDirty(*Spec);
	ClientUpdateAbilityData(*Spec);
}
#pragma endregion


/*
 * TryActivateAbilitiesByTag only use AssetTags, this will check GetDynamicSpecSourceTags()
 */
bool UAuraAbilitySystemComponent::TryActivateAbilityByDynamicTag(const FGameplayTag& Tag,
	const bool bAllowRemoteActivation)
{
	if (!Tag.IsValid()) return false;
	FScopedAbilityListLock AbilityListLock(*this);
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
