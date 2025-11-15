// Copyright Hung


#include "AbilitySystem/Ability/AuraGameplayAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UAuraGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{	// bool bNotUnlocked = GetCurrentAbilitySpec()->GetDynamicSpecSourceTags().HasTagExact();
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UAuraGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	EnableMovement(false); // Change movement here because C++ ActivateAbility() runs after BP
}

void UAuraGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	EnableMovement(true);
}

// Subclasses of this call anything that requires MarkAbilitySpecDirty() before Super::
void UAuraGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{	// "BeginPlay" logic
	// Super::OnAvatarSet(ActorInfo, Spec); // this is empty in base class
	
	AuraCharacter = Cast<AAuraCharacterBase>(ActorInfo->AvatarActor);
	switch (ActivationPolicy)
	{
	case EAuraActivationPolicy::InputStart: break; // TODO: Can add Tag to spec's Dynamic or SetByCaller here
	case EAuraActivationPolicy::InputHolding: break;
	case EAuraActivationPolicy::OnSpawn:
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
		break;
	}
	
	FGameplayAbilitySpec* AbilitySpec = GetCurrentAbilitySpec();
	if (AuraAbilityTag.IsValid()) AbilitySpec->GetDynamicSpecSourceTags().AddTag(AuraAbilityTag);
	ActorInfo->AbilitySystemComponent->MarkAbilitySpecDirty(*AbilitySpec);
}

void UAuraGameplayAbility::EnableMovement(const bool bEnable)
{
	if (bStopRotation)
	{
		AuraCharacter->GetCharacterMovement()->RotationRate = bEnable ? AuraCharacter->BaseRotationRate : FRotator();
	}
}

FGameplayTagContainer& UAuraGameplayAbility::AddGenericAssetTags(FGameplayTagContainer& Tags)
{
	Tags.AddTag(AuraGameplayTags::Generic_Ability_Cancelable);
	// Tags.AddTag(AuraGameplayTags::Generic_Ability_Blockable);
	return Tags;
}
void UAuraGameplayAbility::SetBaseCancelBlock()
{
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Generic_Ability_Cancelable);
	// BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Generic_Ability_Blockable);
	ActivationOwnedTags.AddTag(AuraGameplayTags::Character_State_Ability);
	ActivationBlockedTags.AddTag(AuraGameplayTags::Character_State_Ability);
}
