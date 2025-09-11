// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraLibrary.h"
#include "AbilitySystem/Abilities/CostCooldownAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Character/AuraCharacter.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// This is only called on server -> needs to convert to UFUNCTION(Client, Reliable)
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer); // Asset Tags
	EffectAssetTags.Broadcast(TagContainer); // Broadcast to OverlayWidgetController.cpp ASC->EffectAssetTags.AddLambda()
}


void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UCostCooldownAbility>>& StartupAbilities)
{
	for (const TSubclassOf AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTagFast(AbilityClass.GetDefaultObject()->StartupInputTag); // Add Tag to Spec
		AbilitySpec.GetDynamicSpecSourceTags().AddTagFast(AuraGameplayTags::Ability_Status_Unlocked);
		GiveAbility(AbilitySpec);
	}
}
void UAuraAbilitySystemComponent::AddCharacterPassives(const TArray<TSubclassOf<UAuraGameplayAbility>>& StartupPassives)
{
	for (const TSubclassOf AbilityClass : StartupPassives)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec); // Passives need to be activated first (ex. GA_ListenForEvent with WaitGameplayEvent, ...)
	}
}
void UAuraAbilitySystemComponent::UnlockAbilityByLevel(const int32 CharacterLevel)
{
	if (const UAbilityDataAsset* AbilityData = UAuraLibrary::GetAbilityDataAsset(this))
	{
		for (const FAuraAbilityData& Data : AbilityData->AbilityDataList)
		{
			if (!Data.AbilityTag.IsValid() || CharacterLevel < Data.LevelRequirement
				|| GetSpecFromAssetTag(Data.AbilityTag) /* if this ability is already Activatable */
				) continue;

			FGameplayAbilitySpec AbilitySpec(Data.AbilityClass, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Ability_Status_Eligible);
			GiveAbility(AbilitySpec);
		}
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
void UAuraAbilitySystemComponent::ReduceCooldownByTag(const FGameplayTagContainer& TagContainer, const float Amount, const float Percent)
{
	if (!GetAvatarActor()->HasAuthority()) return;
	const FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(TagContainer);
	for (const FActiveGameplayEffectHandle& Handle : GetActiveEffects(Query))
	{
		if (const FActiveGameplayEffect* ActiveGameplayEffect = GetActiveGameplayEffect(Handle))
		{
			const float TimeRemaining = ActiveGameplayEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			float NewTime = .01f; // Cannot apply effect with 0 duration
			if (Amount > 0.f) NewTime = FMath::Max(TimeRemaining - Amount, NewTime);
			if (Percent > 0.f) NewTime = FMath::Max(NewTime * (1 - Percent), NewTime);

			if (const TSubclassOf<UGameplayEffect> CooldownEffectClass = ActiveGameplayEffect->Spec.Def->GetClass())
			{
				FGameplayEffectSpecHandle NewSpecHandle = MakeOutgoingSpec(CooldownEffectClass,
					ActiveGameplayEffect->Spec.GetLevel(), ActiveGameplayEffect->Spec.GetEffectContext());
				NewSpecHandle.Data->SetDuration(NewTime, true);
				NewSpecHandle.Data->DynamicGrantedTags = ActiveGameplayEffect->Spec.DynamicGrantedTags;

				RemoveActiveGameplayEffect(Handle);
				ApplyGameplayEffectSpecToSelf(*NewSpecHandle.Data);
			}
		}
	}
}
#pragma endregion


/*
 * ===========================================================================================================
 */
#pragma region Activate/Give/Remove Ability
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
	if (const FAuraAbilityData* Data = UAuraLibrary::FindAbilityDataByTags(this, AbilitySpec.Ability->GetAssetTags()))
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
void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const TArray<FPointAllocation>& PointsAllocated,
	AAuraPlayerState* AuraPS)
{
	for (const auto& [AttributeTag, AddedPoints] : PointsAllocated)
	{
		if (AddedPoints > AuraPS->GetAttributePoints() || AddedPoints < 0) return;
		FGameplayEventData Payload;
		Payload.EventTag = AttributeTag;
		Payload.EventMagnitude = AddedPoints;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), Payload.EventTag, Payload);
		AuraPS->AddToAttributePoints(-Payload.EventMagnitude);
	}
	ClientFinishUpgradeAttribute(AuraPS);
}
void UAuraAbilitySystemComponent::ClientFinishUpgradeAttribute_Implementation(const AAuraPlayerState* AuraPS)
{
	AuraPS->OnApplyingStatFinishedDelegate.Broadcast();
}


void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag, AAuraPlayerState* AuraPS)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAssetTag(AbilityTag))
	{
		if (AuraPS->GetSpellPoints() < 1) return;
		AuraPS->AddToSpellPoints(-1);

		if (Spec->GetDynamicSpecSourceTags().RemoveTag(AuraGameplayTags::Ability_Status_Eligible)) // need to spend point to unlock
		{
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
	const FGameplayTag& SlotToSwap = GetInputFromSpec(Spec); // if Spec is assigned to any input
	if (SlotToSwap.MatchesTagExact(SlotTag)) return;
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& OtherSpec : GetActivatableAbilities())
	{
		if (OtherSpec.GetDynamicSpecSourceTags().RemoveTag(SlotTag))
		{
			if (SlotToSwap.IsValid()) OtherSpec.GetDynamicSpecSourceTags().AddTag(SlotToSwap);
			MarkAbilitySpecDirty(OtherSpec);
			ClientUpdateAbilityData(OtherSpec);
		}
	}
	if (SlotToSwap.IsValid()) Spec->GetDynamicSpecSourceTags().RemoveTag(SlotToSwap);
	Spec->GetDynamicSpecSourceTags().AddTag(SlotTag);
	MarkAbilitySpecDirty(*Spec);
	ClientUpdateAbilityData(*Spec);
}
#pragma endregion
