// Copyright Hung


#include "AbilitySystem/Ability/DeathAbility.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "BrainComponent.h"
#include "AbilitySystem/Ability/SummonAbility.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "AI/AuraAIController.h"
#include "Character/AuraCharacterBase.h"
#include "Character/AuraEnemy.h"
#include "Game/AuraGameMode.h"

UDeathAbility::UDeathAbility()
{
	ActivationPolicy = EAuraActivationPolicy::OnSpawn;

	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
}

void UDeathAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	KillAllSummons(AuraCharacter); /** If is summoner, Kill all summons */
	/** If is a summon, remove self from summoner */
	AAuraCharacterBase* SpawnerActor = Cast<AAuraCharacterBase>(AuraCharacter->GetOwner());
	if (!SpawnerActor) SpawnerActor = Cast<AAuraCharacterBase>(AuraCharacter->GetInstigator());
	if (SpawnerActor) SpawnerActor->Summons.RemoveSingleSwap(AuraCharacter);

	if (!HasAuthority(&CurrentActivationInfo)) return;
	const FGameplayEffectContextHandle ContextHandle = GetGrantedByEffectContext();
	/*GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithTags(
		FGameplayTagContainer(AuraTag::Effect_RemoveOnDeath));*/ // RemoveActiveEffectsWithGrantedTags() Clear all
	// GetAbilitySystemComponentFromActorInfo()->RemoveAllGameplayCues()
	if (ContextHandle.IsValid())
	{
		// Send XP To Source on death ======================================================================
		const UCharacterClassDataAsset* ClassDataAsset = UCharacterClassDataAsset::GetFromGameMode(this);
		ClassDataAsset->SendXPToDeathCauser(ContextHandle.GetInstigatorAbilitySystemComponent(), AuraCharacter);
		if (AAuraEnemy* Enemy = Cast<AAuraEnemy>(AuraCharacter))
		{
			Enemy->SetLifeSpan(Enemy->LifeSpan);
			Enemy->AuraAIController->GetBrainComponent()->StopLogic("Death! Disable StateTree");
		}

		AuraCharacter->MulticastHandleDeath();
	}
}

void UDeathAbility::KillAllSummons(AAuraCharacterBase* Chara)
{
	if (!Chara) return;
	if (const AAuraGameMode* AuraGameMode = AAuraGameMode::Get(Chara))
	{
		for (int32 i = Chara->Summons.Num() - 1; i >= 0; i--)  // Iterate backwards so we can remove during loop
		{
			AAuraCharacterBase* Summon = Chara->Summons[i];
			if (!Summon) continue;
			FGameplayEffectSpecHandle SpecHandle = Chara->GetAbilitySystemComponent()->MakeOutgoingSpec(
				AuraGameMode->DeathEffect, 1, FGameplayEffectContextHandle());
			Summon->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}
}
