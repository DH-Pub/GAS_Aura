// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraInputAbility.h"
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


#pragma region Add Startup Abilities
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UAuraInputAbility>>& StartupAbilities)
{
	for (const TSubclassOf AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityClass.GetDefaultObject()->StartupInputTag); // Add Tag to Spec
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Ability_Status_Equipped);
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
#pragma endregion


// ===================================== Input =====================================================================
#pragma region Ability Input
void UAuraAbilitySystemComponent::AbilityInputTagTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag& InputTag, UInputAction* InputAction)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			for (UGameplayAbility* Ability : AbilitySpec.GetAbilityInstances())
			{
				if (UAuraInputAbility* InputAbility = Cast<UAuraInputAbility>(Ability))
				{
					InputAbility->InputAction = InputAction;
					InputAbility->SetAbilityTriggerEvent(TriggerEvent);
				}
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
}

#pragma endregion
// ============================================================================================================


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
	ClientFinishUpgrade(AuraPS);
}
void UAuraAbilitySystemComponent::ClientFinishUpgrade_Implementation(const AAuraPlayerState* AuraPS)
{
	AuraPS->OnApplyingStatFinishedDelegate.Broadcast();
}


FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAssetTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability->GetAssetTags().HasTagExact(AbilityTag)) return &AbilitySpec;
	}
	return nullptr;
}
void UAuraAbilitySystemComponent::UnlockAbilityByLevel(const int32 CharacterLevel)
{
	if (const UAbilityDataAsset* AbilityData = UAuraAbilitySystemLibrary::GetGameModeAbilityDataAsset(GetAvatarActor()))
	{
		for (const FAuraAbilityData& Data : AbilityData->AbilityDataList)
		{
			if (!Data.AbilityTag.IsValid() || CharacterLevel < Data.LevelRequirement || GetSpecFromAssetTag(Data.AbilityTag)) continue;

			FGameplayAbilitySpec AbilitySpec(Data.AbilityClass, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Ability_Status_Eligible);
			GiveAbility(AbilitySpec);
		}
	}
}

const FGameplayTag& UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
	{
		if (Tag.MatchesTag(AuraGameplayTags::Ability)) return Tag;
	}
	return FGameplayTag::EmptyTag;
}
const FGameplayTag& UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(AuraGameplayTags::Ability_Status)) return Tag;
	}
	return FGameplayTag::EmptyTag;
}
void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayAbilitySpec& AbilitySpec,
	const FGameplayTag& StatusTag)
{
	if (AAuraHUD* HUD = UAuraAbilitySystemLibrary::GetAuraHUD(this))
	{
		if (FAuraAbilityData* Data = HUD->AbilityData->FindAbilityDataByTags(AbilitySpec.Ability->GetAssetTags()))
		{
			Data->StatusTag = StatusTag;
			HUD->AbilityDataDelegate.Broadcast(*Data);
		}
	}
}


void UAuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAssetTag(AbilityTag))
	{
		if (const AAuraCharacter* Character = Cast<AAuraCharacter>(GetAvatarActor()))
		{
			if (AAuraPlayerState* PS = Character->GetPlayerState<AAuraPlayerState>())
			{
				if (PS->GetSpellPoints() < 1) return;
				PS->AddToSpellPoints(-1);
				FGameplayTag Status = GetStatusFromSpec(*Spec);
				if (Status.MatchesTagExact(AuraGameplayTags::Ability_Status_Eligible)) // need to spend point to unlock
				{
					Spec->GetDynamicSpecSourceTags().RemoveTag(AuraGameplayTags::Ability_Status_Eligible);
					Status = AuraGameplayTags::Ability_Status_Unlocked;
					Spec->GetDynamicSpecSourceTags().AddTag(Status);
				}
				else if (Status.MatchesTagExact(AuraGameplayTags::Ability_Status_Equipped)
					|| Status.MatchesTagExact(AuraGameplayTags::Ability_Status_Unlocked))
				{
					Spec->Level++; // without MarkAbilitySpecDirty(), client Spec's won't change
				}
				MarkAbilitySpecDirty(*Spec);
				ClientUpdateAbilityStatus(*Spec, Status);
			}
		}
	}
}


#pragma region On Activate/Give/Remove Ability
void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor()))
	{
		if (Character->IsPlayerControlled())
		{
			FScopedAbilityListLock AbilityListLock(*this);
			for (const auto& AbilitySpec : GetActivatableAbilities())
			{
				//TODO: Find something to do with this
			}
		}
	}
}
void UAuraAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor()))
	{
		if (Character->IsPlayerControlled())
		{
			OnGiveAbilityDelegate.Broadcast(AbilitySpec);
			for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
			{
				if (Tag.MatchesTag(AuraGameplayTags::Ability_Status))
				{
					ClientUpdateAbilityStatus(AbilitySpec, Tag);
				}
			}
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
