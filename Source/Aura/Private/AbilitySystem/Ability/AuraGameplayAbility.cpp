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
	// bRetriggerInstancedAbility = true;
}

void UAuraGameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	if (bStopMovement)
	{
		AuraCharacter->GetCharacterMovement()->StopMovementImmediately();
		AuraCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.f;
	}
	if (bStopRotation)	AuraCharacter->GetCharacterMovement()->RotationRate = FRotator();
}
void UAuraGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	EnableMovement();
}

void UAuraGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// "BeginPlay" logic
	AuraCharacter = Cast<AAuraCharacterBase>(ActorInfo->AvatarActor);
	AuraPlayerController = Cast<AAuraPlayerController>(AuraCharacter->GetController());
	if (bActivateAbilityOnGranted) ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}

void UAuraGameplayAbility::EnableMovement()
{
	if (bStopMovement) AuraCharacter->GetCharacterMovement()->MaxWalkSpeed = AuraCharacter->BaseWalkSpeed;
	if (bStopRotation) AuraCharacter->GetCharacterMovement()->RotationRate = AuraCharacter->BaseRotationRate;
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
