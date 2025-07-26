// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Aura/AuraLogChannels.h"
#include "Character/AuraCharacterBase.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// OnGameplayEffectAppliedDelegateToSelf only called on server -> UFUNCTION(Client, Reliable)
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
void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraAbility->StartupInputTag); // Add Tag to Spec
			GiveAbility(AbilitySpec);
			// GiveAbilityAndActivateOnce(AbilitySpec);
		}
	}
}
void UAuraAbilitySystemComponent::AddCharacterPassives(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassives)
{
	for (const TSubclassOf AbilityClass : StartupPassives)
	{
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}
#pragma endregion


#pragma region AbilityReleased/Held
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
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive()) TryActivateAbility(AbilitySpec.Handle);
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


/*void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock AbilityListLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}*/


#pragma region On Give/Remove Ability
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
		if (Character->IsPlayerControlled())
		{
			// AbilitiesGivenDelegate.Broadcast(this);
			OnGiveAbilityDelegate.Broadcast(AbilitySpec);
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
