// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraInputAbility.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Character/AuraCharacter.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/OverlayWidgetController.h"

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
void UAuraAbilitySystemComponent::AbilityInputTagTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag& InputTag)
{
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			if (AbilitySpec.IsActive())
			{
				for (UGameplayAbility* Ability : AbilitySpec.GetAbilityInstances())
				{
					Cast<UAuraInputAbility>(Ability)->SetAbilityTriggerEvent(TriggerEvent);
				}
			}
			switch (TriggerEvent)
			{
			case ETriggerEvent::Ongoing:
			case ETriggerEvent::Triggered:
				// AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Input_TriggerEvent_Triggered);
				TryActivateAbility(AbilitySpec.Handle);
				// AbilitySpec.GetDynamicSpecSourceTags().RemoveTag(AuraGameplayTags::Input_TriggerEvent_Triggered);
				break;
				
			case ETriggerEvent::Started:
				// AbilitySpecInputPressed(AbilitySpec); // InputPressed called if Spec.IsActive()
				break;
				
			case ETriggerEvent::Canceled:
			case ETriggerEvent::Completed:
				AbilitySpecInputReleased(AbilitySpec);
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
	TArray<FActiveGameplayEffectHandle> ActiveCooldownHandles = GetActiveEffects(Query);
	for (const FActiveGameplayEffectHandle& Handle : ActiveCooldownHandles)
	{
		if (const FActiveGameplayEffect* ActiveGameplayEffect = GetActiveGameplayEffect(Handle))
		{
			const float TimeRemaining = ActiveGameplayEffect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			float NewTime = 0.004f; // Cannot apply effect with 0 duration
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

void UAuraAbilitySystemComponent::UpgradeAttribute(const TArray<FPointAllocation>& PointsAllocated)
{
	if (const AAuraCharacter* Character = Cast<AAuraCharacter>(GetAvatarActor()))
	{
		if (AAuraPlayerState* PS = Character->GetPlayerState<AAuraPlayerState>())
		{
			ServerUpgradeAttribute(PointsAllocated, PS);
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
void UAuraAbilitySystemComponent::UpdateAbilityStatuses(const int32 CharacterLevel)
{
	if (UAbilityDataAsset* AbilityData = UAuraAbilitySystemLibrary::GetGameModeAbilityDataAsset(GetAvatarActor()))
	{
		for (FAuraAbilityData& Data : AbilityData->AbilityDataList)
		{
			if (!Data.AbilityTag.IsValid() || CharacterLevel < Data.LevelRequirement || GetSpecFromAssetTag(Data.AbilityTag)) continue;

			FGameplayAbilitySpec AbilitySpec(Data.AbilityClass, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayTags::Ability_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec);
		}
	}
}

/*void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec)) UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
	}
}*/


#pragma region On Activate/Give/Remove Ability
void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (const AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(GetAvatarActor()))
	{
		if (Character->IsPlayerControlled())
		{
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
		if (Character->IsPlayerControlled()) OnGiveAbilityDelegate.Broadcast(AbilitySpec);
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
