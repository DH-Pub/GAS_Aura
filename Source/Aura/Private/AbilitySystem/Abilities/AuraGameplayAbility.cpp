// Copyright Hung


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "Character/AuraCharacterBase.h"
#include "Interaction/CombatInterface.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// bRetriggerInstancedAbility = true;
}

const FGameplayTagContainer* UAuraGameplayAbility::GetCooldownTags() const
{
	if (!CooldownTags.IsValid()) return Super::GetCooldownTags();

	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the cooldown tags change (to a different slot)
	if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags())
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UAuraGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (!CooldownTags.IsValid()) {Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo); return;}
	
	if (UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data->SetSetByCallerMagnitude(CooldownTags.GetByIndex(0), CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		// Use MMC
		FActiveGameplayEffectHandle ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void UAuraGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	if (AuraCharacterFromActorInfo == nullptr)
	{
		AuraCharacterFromActorInfo = Cast<AAuraCharacterBase>(GetAvatarActorFromActorInfo());
		if (AuraCharacterFromActorInfo == nullptr) EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
	}
}
