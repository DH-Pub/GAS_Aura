// Copyright Hung


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Character/AuraCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"

UAuraGameplayAbility::UAuraGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	// bRetriggerInstancedAbility = true;
}

void UAuraGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (bMaxSpeedZeroedOnActivated)
	{
		//TODO: remember that this may cause rubberbanding issue and do not use Deactivate
		AuraCharacterFromActorInfo->GetCharacterMovement()->MaxWalkSpeed = 0.f;
	}
}

void UAuraGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (bMaxSpeedZeroedOnActivated)
	{
		AuraCharacterFromActorInfo->GetCharacterMovement()->MaxWalkSpeed = AuraCharacterFromActorInfo->BaseWalkSpeed;
	}
}

void UAuraGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// "BeginPlay" logic
	AuraCharacterFromActorInfo = Cast<AAuraCharacterBase>(ActorInfo->AvatarActor);
	AuraPlayerController = Cast<AAuraPlayerController>(AuraCharacterFromActorInfo->GetController());
}

FGameplayTagContainer& UAuraGameplayAbility::AddGenericAssetTags(FGameplayTagContainer& Tags)
{
	Tags.AddTag(AuraGameplayTags::Generic_Ability_Blockable);
	Tags.AddTag(AuraGameplayTags::Generic_Ability_Cancelable);
	return Tags;
}
void UAuraGameplayAbility::SetGenericCancelBlockAbility()
{
	FGameplayTagContainer CancelTags(AuraGameplayTags::Generic_Ability_Cancelable);
	CancelAbilitiesWithTag = CancelTags;
	FGameplayTagContainer BlockTags(AuraGameplayTags::Generic_Ability_Blockable);
	BlockAbilitiesWithTag = BlockTags;
}
