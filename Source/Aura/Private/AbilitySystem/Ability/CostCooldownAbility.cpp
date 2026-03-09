// Copyright Hung


#include "AbilitySystem/Ability/CostCooldownAbility.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Effect/CostCooldownEffect.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Duration, "Cooldown.Duration")

UCostCooldownAbility::UCostCooldownAbility()
{
	FGameplayTagContainer Tags(AuraGameplayTags::Generic_Ability_Blockable);
	Tags.AddTagFast(AuraGameplayTags::Generic_Ability_Cancelable);
	SetAssetTags(Tags);

	BlockAbilitiesWithTag.AddTag(AuraGameplayTags::Generic_Ability_Blockable);

	ActivationBlockedTags.AddTag(AuraGameplayTags::State_HitReact);
	ActivationBlockedTags.AddTag(AuraGameplayTags::State_Death);
	// ActivationRequiredTags.AddTag();

	CostGameplayEffectClass = UCostEffect::StaticClass();
}

const FGameplayTagContainer* UCostCooldownAbility::GetCooldownTags() const
{	// For CheckCooldown and Get Time Remains
	if (CooldownTags.IsEmpty()) return Super::GetCooldownTags();
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the CD tags change (to another slot)
	MutableTags->AppendTags(CooldownTags); // MutableTags->AddTag(AuraAbilityTag);
	if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags()) MutableTags->AppendTags(*ParentTags);
	return MutableTags;
}

bool UCostCooldownAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bResult = Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
	if (!bResult)
	{
		//TODO: Send Error Data to UI
	}
	return bResult;
}

void UCostCooldownAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownTags.IsEmpty())
	{
		Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
		return;
	}
	const int32 Level = GetAbilityLevel(Handle, ActorInfo);
	const float BaseCD = CooldownDuration.GetValueAtLevel(Level);
	if (BaseCD < .006f) return;
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(UCooldownEffect::StaticClass(), Level);
	SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags); // if DurMag:SetByCaller ->SetByCallerMagnitudes
	SpecHandle.Data->SetSetByCallerMagnitude(TAG_Cooldown_Duration, BaseCD);
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

bool UCostCooldownAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{	// return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		// SCOPE_CYCLE_COUNTER(STAT_GameplayEffectsCanApplyAttributeModifiers);
		FGameplayEffectSpec	Spec(CostGE, MakeEffectContext(Handle, ActorInfo), GetAbilityLevel(Handle, ActorInfo));

		FGameplayEffectCustomExecutionOutput ExecutionOutput;
		GetCostExecutionOutput(ASC, Spec, ExecutionOutput);
		for (FGameplayModifierEvaluatedData& CurExecMod : ExecutionOutput.GetOutputModifiersRef())
		{
			const float CurrentValue = ASC->GetNumericAttribute(CurExecMod.Attribute);
			if (CurrentValue + CurExecMod.Magnitude < 0.f)
			{
				const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
				if (OptionalRelevantTags && CostTag.IsValid()) OptionalRelevantTags->AddTag(CostTag);
				return false;
			}
		}

	#pragma region UGameplayAbility Default // Using Modifiers
		for (const FGameplayModifierInfo& ModDef : Spec.Def->Modifiers)
		{	// It only makes sense to check additive operators
			if (ModDef.ModifierOp != EGameplayModOp::Additive || !ModDef.Attribute.IsValid()) continue;
			// Attribute.GetNumericValueChecked(ASC->GetAttributeSubobject(Attribute.GetAttributeSetClass()));
			const float CurrentValue = ASC->GetNumericAttribute(ModDef.Attribute);
			float CostValue; ModDef.ModifierMagnitude.AttemptCalculateMagnitude(Spec, CostValue);
			if (CurrentValue + CostValue < 0.f)
			{
				const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
				if (OptionalRelevantTags && CostTag.IsValid()) OptionalRelevantTags->AddTag(CostTag);
				return false;
			}
		}
	#pragma endregion
	}
	return true;
}

void UCostCooldownAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

}

void UCostCooldownAbility::GetCostExecutionOutput(UAbilitySystemComponent* ASC, FGameplayEffectSpec& Spec,
	FGameplayEffectCustomExecutionOutput& ExecutionOutput) const
{
	const FPredictionKey PredictionKey = ASC->GetPredictionKeyForNewAction();
	for (const FGameplayEffectExecutionDefinition& CurExecDef : Spec.Def->Executions)
	{
		if (!CurExecDef.CalculationClass) continue;
		const UGameplayEffectExecutionCalculation* ExecCDO = CurExecDef.CalculationClass->
			GetDefaultObject<UGameplayEffectExecutionCalculation>();
		FGameplayEffectCustomExecutionParameters ExecutionParams(Spec, CurExecDef.CalculationModifiers, ASC,
			CurExecDef.PassedInTags, PredictionKey);
		ExecCDO->Execute(ExecutionParams, ExecutionOutput);
	}
}

#pragma region GetCostCooldowns
void UCostCooldownAbility::GetCost(FAbilityDetails& Details) const
{
	auto GetCost = [this, &Details](const FGameplayTag& Tag)
	{
		if (const FScalableFloat* Cost = AbilityCosts.Find(Tag))
		{
			const float Result = Cost->GetValueAtLevel(Details.Level);
			return static_cast<int32>(Result * 10.f) / 10.f;
		}
		return 0.f;
	};
	Details.CostMana = GetCost(AuraGameplayTags::Attributes_Vital_Mana);
	Details.CostHealth = GetCost(AuraGameplayTags::Attributes_Vital_Health);
}
void UCostCooldownAbility::GetCooldownAndReduction(FAbilityDetails& Details) const
{
	/*const TSubclassOf<UCooldownEffect> CooldownEffect = UCooldownEffect::StaticClass();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownEffect, Details.Level);
	Details.BaseCooldown = CooldownDuration.GetValueAtLevel(Details.Level);
	if (const TSubclassOf<UGameplayModMagnitudeCalculation> ModCalcClass =
		CooldownEffect->GetDefaultObject<UCooldownEffect>()->DurationMagnitude.GetCustomMagnitudeCalculationClass())
	{	if (const UMMC_CooldownDuration* CalcCDO = ModCalcClass->GetDefaultObject<UMMC_CooldownDuration>())
		{Details.CalculatedCooldown = CalcCDO->CalculateBaseMagnitude(Spec);}
	}*/
	Details.BaseCooldown = static_cast<int32>(CooldownDuration.GetValueAtLevel(Details.Level) * 10.f) / 10.f;
	const UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	Details.CooldownReduction = UMMC_CooldownDuration::GetCooldownReductionPercent(
		ASC->GetNumericAttribute(UAuraAttributeSet::GetIntelligenceAttribute()));

	Details.CalculatedCooldown = Details.BaseCooldown * (1 - Details.CooldownReduction);
	Details.CalculatedCooldown = static_cast<int32>(Details.CalculatedCooldown * 10.f) / 10.f;
}
void UCostCooldownAbility::GetAbilityDetails(FAbilityDetails& Details) const
{
	GetCost(Details);
	GetCooldownAndReduction(Details);
}
#pragma endregion
