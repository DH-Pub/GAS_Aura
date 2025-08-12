// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Character/AuraCharacter.h"
#include "Player/AuraPlayerState.h"

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
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UAuraGameplayAbility>>& StartupAbilities)
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


#pragma region Ability Pressed/Released
void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			// if (!AbilitySpec.IsActive())
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}
#pragma endregion


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


const FGameplayTag* UAuraAbilitySystemComponent::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(AuraGameplayTags::Ability_Status))
		{
			return &Tag;
		}
	}
	return nullptr;
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
				// OnGiveAbilityDelegate.Broadcast(AbilitySpec);
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
