// Copyright Hung


#include "AbilitySystem/Abilities/CostCooldownAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

const FGameplayTagContainer* UCostCooldownAbility::GetCooldownTags() const
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

void UCostCooldownAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (!CooldownTags.IsValid()) {Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo); return;}
	
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags);
		SpecHandle.Data->SetSetByCallerMagnitude(CooldownTags.GetByIndex(0), CooldownDuration.GetValueAtLevel(GetAbilityLevel()));
		// Use MMC
		FActiveGameplayEffectHandle ActiveEffectHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

void UCostCooldownAbility::GetCost(float& Mana, float& Health, const int32 InLevel)
{
	Mana = Health = 0.f;
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		for (const FGameplayModifierInfo& Mod : CostGE->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), InLevel);
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Mana);
				Mana *= -1.f;
				// Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, Mana);
			}
			else if (Mod.Attribute == UAuraAttributeSet::GetHealthAttribute())
			{
				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), InLevel);
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Health);
				Health *= -1.f;
				// Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, Health);
			}
		}
	}
}
void UCostCooldownAbility::GetCostOnLevelChanged(float& Mana, float& ManaChanged, float& Health, float& HealthChanged,
	const int32 InLevel, const int32 LevelDelta) const
{
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), InLevel);
		for (const FGameplayModifierInfo& Mod : CostGE->Modifiers)
		{
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Mana);
				Mana *= -1.f;
				SpecHandle.Data->SetLevel(InLevel + LevelDelta);
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, ManaChanged);
				ManaChanged *= -1.f;
				// Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, Mana);
			}
			else if (Mod.Attribute == UAuraAttributeSet::GetHealthAttribute())
			{
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Health);
				Health *= -1.f;
				SpecHandle.Data->SetLevel(InLevel + LevelDelta);
				Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, HealthChanged);
				// Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, Health);
			}
		}
	}
}

float UCostCooldownAbility::GetCooldown(const int32 InLevel)
{
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		const FGameplayEffectModifierMagnitude& ModMag = CooldownGE->DurationMagnitude;
		
	}
	return 0.f;
}
