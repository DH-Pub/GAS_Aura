// Copyright Hung


#include "AbilitySystem/GameplayEffectComponent/DebuffComponent.h"

#include "AbilitySystemComponent.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "GameplayEffect.h"
#include "AbilitySystem/Ability/DamageAbility.h"

bool UDebuffComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer,
	FActiveGameplayEffect& ActiveGE) const
{
	// Super::OnActiveGameplayEffectAdded(ActiveGEContainer, ActiveGE);
	if (ActiveGEContainer.IsNetAuthority()) // We don't allow prediction of expiration (on removed) effects
	{	// When this ActiveGE gets removed, so will our events so no need to unbind this.
		ActiveGE.EventSet.OnEffectRemoved.AddUObject(this,
			&UDebuffComponent::OnActiveGameplayEffectRemoved, &ActiveGEContainer);
	}
	return true;
}

// Apply Debuff here so that there is a buffer time between damage and debuff
void UDebuffComponent::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo,
	FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
	FScopedActiveGameplayEffectLock ActiveScopeLock(*ActiveGEContainer);

	if (ActiveGEContainer->Owner->HasMatchingGameplayTag(AuraGameplayTags::Character_State_Death)) return;
	const FGameplayEffectSpec& Spec = RemovalInfo.ActiveEffect->Spec;
	const UDamageAbility* DamageAbility = Cast<UDamageAbility>(Spec.GetEffectContext().GetAbilityInstance_NotReplicated());
	 const float SourceDebuffChance = DamageAbility->DebuffChance.GetValueAtLevel(Spec.GetLevel());
	if (FMath::RandRange(0.f, 1.f) > SourceDebuffChance) return;
	if (const UGameplayEffect* CurExpiryCDO = DamageAbility->DebuffEffectClass.GetDefaultObject())
	{
		FDamageEffectContext& DamageContext = FAuraEffectContext
			::GetOrMakeContextStructRef<FDamageEffectContext>(Spec.GetContext().Get()); // Reset Context for debuff
		DamageContext.DamageDirection = FVector::ZeroVector;
		FGameplayEffectSpec NewSpec;
		NewSpec.DynamicGrantedTags.AddTag(DamageAbility->DebuffType);
		NewSpec.SetDuration(DamageAbility->DebuffDuration.GetValueAtLevel(Spec.GetLevel()), true);
		NewSpec.InitializeFromLinkedSpec(CurExpiryCDO, Spec);
		NewSpec.Period = DamageAbility->DebuffPeriod.GetValueAtLevel(DamageAbility->GetAbilityLevel());

		ActiveGEContainer->Owner->ApplyGameplayEffectSpecToSelf(NewSpec);
	}
}
