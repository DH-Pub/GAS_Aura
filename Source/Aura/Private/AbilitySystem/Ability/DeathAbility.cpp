// Copyright Hung


#include "AbilitySystem/Ability/DeathAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "BrainComponent.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "AI/AuraAIController.h"
#include "Character/AuraCharacterBase.h"
#include "Character/AuraEnemy.h"

UDeathAbility::UDeathAbility()
{
	CancelAbilitiesWithTag = FGameplayTagContainer(AuraGameplayTags::Generic_Ability_Cancelable);
	CancelAbilitiesWithTag.AddTag(AuraGameplayTags::Character_State_HitReact);

	ActivationOwnedTags.AddTag(AuraGameplayTags::Character_State_Death); // Adds Tag to ASC on Activation 

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;

	bStopRotation = bStopMovement = true;

	const int32 Idx = AbilityTriggers.Add(FAbilityTriggerData());
	AbilityTriggers[Idx].TriggerTag = AuraGameplayTags::Character_State_Death;
	AbilityTriggers[Idx].TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
}

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (TriggerEventData == nullptr) return;
	if (AuraCharacter != AuraCharacter->GetInstigator())
	{	// Summoned have Instigator Set as the Summoner
		if (AAuraCharacterBase* AuraInstigator = Cast<AAuraCharacterBase>(AuraCharacter->GetInstigator()))
		{
			AuraInstigator->Summons.RemoveSingleSwap(AuraCharacter);
		}
	}
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithGrantedTags(
		FGameplayTagContainer(AuraGameplayTags::Debuff_Type)); // Clear all debuffs

	if (!HasAuthority(&GetCurrentActivationInfoRef())) return;
	// Send XP To Source on death =====================================================================================
	const UCharacterClassDataAsset* ClassDataAsset = UCharacterClassDataAsset::GetFromGameMode(this);
	AActor* Causer = TriggerEventData->ContextHandle.GetInstigatorAbilitySystemComponent()->GetAvatarActor();
	if (Causer != AuraCharacter) ClassDataAsset->SendXPToDeathCauser(Causer, AuraCharacter); // cause is not itself
	if (AAuraEnemy* Enemy = Cast<AAuraEnemy>(AuraCharacter))
	{
		Enemy->SetLifeSpan(Enemy->LifeSpan);
		Enemy->AuraAIController->GetBrainComponent()->StopLogic("Death! Disable StateTree");
	}

	const FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(TriggerEventData->ContextHandle);
	FVector Impulse;
	if (const UDamageAbility* DamageAbility = Cast<UDamageAbility>(AuraContext->GetAbilityInstance_NotReplicated()))
	{
		if (const FDamageEffectContext* DamageContext = AuraContext->GetStruct<FDamageEffectContext>())
		{
			Impulse = DamageContext->DamageDirection * DamageAbility->DeathImpulseMagnitude;
		}
	}
	AuraCharacter->MulticastHandleDeath(Impulse);
}
