// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "AbilitySystem/Ability/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Character/AuraPlayer.h"
#include "Player/AuraPlayerState.h"

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
	for (const TSubclassOf AbilityClass : StartupActives)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		GiveAbility(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::UnlockAbilityByLevel(const int32 CharacterLevel)
{
	for (const FAuraAbilityData& Data : UAbilityDataAsset::GetFromGameState(this)->AbilityDataList)
	{	 /* not enough lv or already has ability */
		if (CharacterLevel < Data.LevelRequirement || GetSpecFromAssetTag(Data.AbilityTag)) continue;

		FGameplayAbilitySpec AbilitySpec(Data.AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Ability_Status_Eligible);
		GiveAbility(AbilitySpec);
	}
}

// ===================================== Input =====================================================================
#pragma region Activate Ability by Input =============================
void UAuraAbilitySystemComponent::AbilityInputTagTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag& InputTag,
	UInputAction* InputAction)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)) continue;
		if (UCostCooldownAbility* InputAbility = Cast<UCostCooldownAbility>(AbilitySpec.NonReplicatedInstances[0]))
		{
			InputAbility->InputAction = InputAction;
			InputAbility->SetAbilityTriggerEvent(TriggerEvent);
		}
		switch (TriggerEvent)
		{
		case ETriggerEvent::Started:
		case ETriggerEvent::Ongoing:
		case ETriggerEvent::Triggered:
			TryActivateAbility(AbilitySpec.Handle);
			break;
			
		case ETriggerEvent::Canceled:
		case ETriggerEvent::Completed:
			AbilitySpecInputReleased(AbilitySpec); // only called if Spec.IsActive() same as AbilitySpecInputPressed
			MarkAbilitySpecDirty(AbilitySpec);
			break;
		
		case ETriggerEvent::None: break;
		}
	}
}
#pragma endregion
// ============================================================================================================


FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAssetTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& ActivatableSpec : GetActivatableAbilities())
	{
		if (ActivatableSpec.Ability->GetAssetTags().HasTagExact(AbilityTag)) return &ActivatableSpec;
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


#pragma region Abilities Functions ==================================
void UAuraAbilitySystemComponent::ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, const float Amount, float Percent)
{
	if (!GetAvatarActor()->HasAuthority()) return;
	for (const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
		const FActiveGameplayEffectHandle& Handle : ActiveGameplayEffects.GetActiveEffects(Query))
	{
		FActiveGameplayEffect* ActiveEffect = ActiveGameplayEffects.GetActiveGameplayEffect(Handle);
		if (ActiveEffect == nullptr) continue;
		const float CurrentWorldTime = GetWorld()->GetTimeSeconds();
		const float TimeRemaining = ActiveEffect->GetTimeRemaining(CurrentWorldTime);
		float NewTime = FMath::Max(TimeRemaining - Amount, 0.007f); // Cannot apply effect with 0 duration
		if (Percent > UE_KINDA_SMALL_NUMBER)
		{
			Percent = FMath::Min(Percent, 0.6); // Max Cooldown 60%
			NewTime = FMath::Max(NewTime * (1 - Percent), NewTime);
		}

		if (const TSubclassOf<UGameplayEffect> CooldownEffectClass = ActiveEffect->Spec.Def->GetClass())
		{
			FGameplayEffectSpecHandle NewSpecHandle = MakeOutgoingSpec(CooldownEffectClass,
				ActiveEffect->Spec.GetLevel(), ActiveEffect->Spec.GetEffectContext());
			NewSpecHandle.Data->SetDuration(NewTime, true);
			// DON'T copy SetByCallerTagMagnitudes
			NewSpecHandle.Data->DynamicGrantedTags = ActiveEffect->Spec.DynamicGrantedTags;

			RemoveActiveGameplayEffect(Handle);
			ApplyGameplayEffectSpecToSelf(*NewSpecHandle.Data);
		}
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
	if (const FAuraAbilityData* Data = UAbilityDataAsset::GetAbilityFromGameState(
		this, AbilitySpec.Ability->GetAssetTags()))
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
		if (AddedPoints > AuraPS->GetAttributePoints() || AddedPoints < 1) return;
		Data->AttributeTags.Add(AttributeTag);
		Data->AttributeMagnitudes.Add(AddedPoints); AuraPS->AddToAttributePoints(-AddedPoints);
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
	if (FGameplayAbilitySpec* Spec = GetSpecFromAssetTag(AbilityTag))
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
	FGameplayAbilitySpec* Spec = GetSpecFromAssetTag(AbilityTag);
	if (Spec == nullptr || !Spec->GetDynamicSpecSourceTags().HasTagExact(AuraGameplayTags::Ability_Status_Unlocked)) return;
	const FGameplayTag& SlotToSwap = GetInputFromSpec(Spec); // Store Spec's input if there is any
	if (SlotToSwap.MatchesTagExact(SlotTag)) return; // if Ability is moved to the same Slot
	if (SlotTag.MatchesTag(AuraGameplayTags::Input_Combat_Passive) /*Server side check valid type*/
		!= Cast<UAuraGameplayAbility>(Spec->Ability)->bActivateAbilityOnGranted) return;
	if (SlotTag.IsValid()) // if not, Slot will just be removed
	{
		FScopedAbilityListLock AbilityListLock(*this);
		for (FGameplayAbilitySpec& OtherSpec : GetActivatableAbilities())
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
