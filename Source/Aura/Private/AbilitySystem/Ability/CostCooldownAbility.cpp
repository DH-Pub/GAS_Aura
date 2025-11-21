// Copyright Hung


#include "AbilitySystem/Ability/CostCooldownAbility.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Effect/CostCooldownEffect.h"

UCostCooldownAbility::UCostCooldownAbility()
{
	FGameplayTagContainer DefaultAssetTags;
	SetAssetTags(AddGenericAssetTags(DefaultAssetTags));
	SetBaseCancelBlock();
	// ActivationOwnedTags.AddTag(AuraGameplayTags::Ability_Fire_FireBolt);
	ActivationBlockedTags.AddTag(AuraGameplayTags::Character_State_HitReact);
	ActivationBlockedTags.AddTag(AuraGameplayTags::Character_State_Death);
	// ActivationRequiredTags.AddTag();

	CostGameplayEffectClass = UCostEffect::StaticClass();
}

const FGameplayTagContainer* UCostCooldownAbility::GetCooldownTags() const
{	// For CheckCooldown and Get Time Remains
	if (!AuraAbilityTag.IsValid()) return Super::GetCooldownTags();
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the CD tags change (to another slot)
	/*if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags())
	{	// if GetCooldownGameplayEffect() exists, GE->GetGrantedTags()
		MutableTags->AppendTags(*ParentTags);
	}*/
	MutableTags->AddTag(AuraAbilityTag);
	return MutableTags;
}

void UCostCooldownAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (AuraAbilityTag.IsValid())
	{
		if (CooldownGameplayEffectClass == nullptr) return; // Using UMMC_CooldownDuration for DurMag
		if (CooldownDuration.GetValueAtLevel(GetAbilityLevel()) < UE_KINDA_SMALL_NUMBER) return;
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass, GetAbilityLevel());
		SpecHandle.Data->DynamicGrantedTags.AddTag(AuraAbilityTag); // if DurMag:SetByCaller ->SetByCallerMagnitudes
		if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
		{	// ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle)
			UAbilitySystemComponent* const ASC = ActorInfo->AbilitySystemComponent.Get();
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get(), ASC->GetPredictionKeyForNewAction());
		}
	}
	else Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
}

void UCostCooldownAbility::GetCost(FAbilityDetails& Details) const
{
	const UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (CostGE == nullptr) return;
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), Details.Level);
	for (const FGameplayModifierInfo& Mod : CostGE->Modifiers)
	{
		if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
		{
			Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Details.CostMana);
			Details.CostMana = static_cast<int32>(Details.CostMana * -10.f) / 10.f;
			// Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, Mana);
		}
		else if (Mod.Attribute == UAuraAttributeSet::GetHealthAttribute())
		{
			Mod.ModifierMagnitude.AttemptCalculateMagnitude(*SpecHandle.Data, Details.CostHealth);
			Details.CostHealth = static_cast<int32>(Details.CostHealth * -10.f) / 10.f;
		}
	}
}
void UCostCooldownAbility::GetCooldownAndReduction(FAbilityDetails& Details) const
{
	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE == nullptr) return;
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), Details.Level);
	const FGameplayEffectModifierMagnitude& ModMag = CooldownGE->DurationMagnitude;
	// const UGameplayModMagnitudeCalculation* CalcCDO = ModMag.GetCustomMagnitudeCalculationClass()->GetDefaultObject<>();
	if (const UMMC_CooldownDuration* CalcCDO = ModMag.GetCustomMagnitudeCalculationClass()->GetDefaultObject<UMMC_CooldownDuration>())
	{
		auto [Cooldown, ReductionPercent] = CalcCDO->GetBaseCooldownAndReductionPercent(*SpecHandle.Data);
		Details.BaseCooldown = static_cast<int32>(Cooldown * 10.f) / 10.f;
		Details.CooldownReduction = ReductionPercent;

		Details.CalculatedCooldown = Details.BaseCooldown * (1 - Details.CooldownReduction);
		Details.CalculatedCooldown = static_cast<int32>(Details.CalculatedCooldown * 10.f) / 10.f;
	}
}
void UCostCooldownAbility::GetAbilityDetailsCostCooldown(FAbilityDetails& Details) const
{
	GetCost(Details);
	GetCooldownAndReduction(Details);
}
