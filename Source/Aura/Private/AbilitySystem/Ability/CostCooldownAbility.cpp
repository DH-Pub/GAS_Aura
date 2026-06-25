// Copyright Hung


#include "AbilitySystem/Ability/CostCooldownAbility.h"

#include "AbilitySystemGlobals.h"
#include "AuraTag.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/ModMagCalc/MMC_CooldownDuration.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Cooldown_Duration, "Cooldown.Duration")

UCostCooldownAbility::UCostCooldownAbility()
{
	FGameplayTagContainer Tags(AuraTag::Ability_Cancelable_Generic);
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(AuraTag::State_Ability);

	ActivationBlockedTags.AddTag(AuraTag::State_HitReact);
	ActivationBlockedTags.AddTag(AuraTag::State_Death);
	ActivationBlockedTags.AddTag(AuraTag::State_Ability);
	// ActivationRequiredTags.AddTag();
}

const FGameplayTagContainer* UCostCooldownAbility::GetCooldownTags() const
{	// For CheckCooldown and Get Time Remains
	if (CooldownTags.IsEmpty()) return Super::GetCooldownTags();
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the CD tags change (to another slot)
	MutableTags->AppendTags(CooldownTags);
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
	if (const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect())
	{
		const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), Level);
		SpecHandle.Data->DynamicGrantedTags.AppendTags(CooldownTags); // if DurMag:SetByCaller ->SetByCallerMagnitudes
		SpecHandle.Data->SetSetByCallerMagnitude(TAG_Cooldown_Duration, BaseCD);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}

bool UCostCooldownAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const float Level = GetAbilityLevel(Handle, ActorInfo);
		FGameplayEffectSpec	Spec(CostGE, MakeEffectContext(Handle, ActorInfo), Level);

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

		Spec.CaptureAttributeDataFromTarget(ASC); // needed for modifier, not execution
		Spec.CalculateModifierMagnitudes();
		for(int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
		{
			const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIdx];
			const FModifierSpec& ModSpec = Spec.Modifiers[ModIdx];

			if (ModDef.ModifierOp == EGameplayModOp::Additive) // It only makes sense to check additive operators
			{
				if (!ModDef.Attribute.IsValid()) continue;
				for (const UAttributeSet* Set : ASC->GetSpawnedAttributes())
				{	// ASC->GetAttributeSubobject(ModDef.Attribute.GetAttributeSetClass());
					if (!Set || !Set->IsA(ModDef.Attribute.GetAttributeSetClass())) continue;
					const float CurrentValue = ModDef.Attribute.GetNumericValueChecked(Set);
					const float CostValue = ModSpec.GetEvaluatedMagnitude();
					if (CurrentValue + CostValue < 0.f)
					{
						const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;
						if (OptionalRelevantTags && CostTag.IsValid()) OptionalRelevantTags->AddTag(CostTag);
						return false;
					}
					break;
				}
			}
		}
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
	/*
	 * Spec.CaptureAttributeDataFromTarget(ASC); // NOT needed for UGameplayEffectExecutionCalculation to get Attribute
	 * Unlike UGameplayModMagnitudeCalculation
	 */
	const FPredictionKey PredictionKey = ASC ? ASC->GetPredictionKeyForNewAction() : FPredictionKey();
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
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		UAbilitySystemComponent* ASC = Details.AbilitySystemComponent.Get();
		FGameplayEffectContextHandle ContextHandle = ASC ? ASC->MakeEffectContext() :
			FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
		ContextHandle.SetAbility(this);
		FGameplayEffectSpec	Spec(CostGE, ContextHandle, Details.Level);
		FGameplayEffectCustomExecutionOutput ExecutionOutput;
		GetCostExecutionOutput(ASC, Spec, ExecutionOutput); // ASC can be nullptr => Attributes for Execution: 0
		for (FGameplayModifierEvaluatedData& Data : ExecutionOutput.GetOutputModifiersRef())
		{
			if (Data.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				Details.CostMana = -Data.Magnitude;
			}
			else if (Data.Attribute == UAuraAttributeSet::GetHealthAttribute())
			{
				Details.CostHealth = -Data.Magnitude;
			}
		}
	}
}
void UCostCooldownAbility::GetCooldown(FAbilityDetails& Details) const
{
	const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect();
	if (!CooldownEffect) return;
	UAbilitySystemComponent* ASC = Details.AbilitySystemComponent.Get();
	FGameplayEffectContextHandle ContextHandle = ASC ? ASC->MakeEffectContext() :
		FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
	ContextHandle.SetAbility(this);
	FGameplayEffectSpec	Spec(CooldownEffect, ContextHandle, Details.Level);
	Spec.SetByCallerTagMagnitudes.Add(TAG_Cooldown_Duration,
		Details.BaseCooldown = CooldownDuration.GetValueAtLevel(Details.Level));
	Spec.CaptureAttributeDataFromTarget(ASC); // ASC can be nullptr and return 0 for Attribute
	if (const TSubclassOf<UGameplayModMagnitudeCalculation> ModCalcClass =
		CooldownEffect->DurationMagnitude.GetCustomMagnitudeCalculationClass())
	{	/*if (const UMMC_CooldownDuration* CalcCDO = ModCalcClass->GetDefaultObject<UMMC_CooldownDuration>())
		{Details.Cooldown = CalcCDO->CalculateBaseMagnitude(Spec);}*/
		Details.Cooldown = ModCalcClass.GetDefaultObject()->CalculateBaseMagnitude(Spec);
	}
}
void UCostCooldownAbility::GetAbilityDetails(FAbilityDetails& Details) const
{
	Super::GetAbilityDetails(Details);
	GetCost(Details);
	GetCooldown(Details);
}
#pragma endregion
