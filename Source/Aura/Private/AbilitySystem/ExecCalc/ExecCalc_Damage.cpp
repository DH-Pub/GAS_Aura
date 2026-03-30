// Copyright Hung


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemGlobals.h"
#include "AuraTag.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "AuraEffectTypes.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "Character/AuraCharacterBase.h"

struct FAuraDamageStatics
{	// DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance); //Lyra/GASDoc: Create CriticalHitChanceProperty and CriticalHitChanceDef
	AURA_ATTR_DEFINE_DEF(SourceCritChanceDef, GetCriticalHitChanceAttribute(), Source, false)
	AURA_ATTR_DEFINE_DEF(SourceCritDamageDef, GetCriticalHitDamageAttribute(), Source, false)
	AURA_ATTR_DEFINE_DEF(SourceArmorPenDef, GetArmorPenetrationAttribute(), Source, false)

	AURA_ATTR_DEFINE_DEF(TargetArmorDef, GetArmorAttribute(), Target, false)
	AURA_ATTR_DEFINE_DEF(TargetBlockChanceDef, GetBlockChanceAttribute(), Target, false)
	AURA_ATTR_DEFINE_DEF(TargetCritHitResDef, GetCriticalHitResistanceAttribute(), Target, false)

	AURA_ATTR_DEFINE_DEF(TargetFireResDef, GetFireResistanceAttribute(), Target, false)
	AURA_ATTR_DEFINE_DEF(TargetLightningResDef, GetLightningResistanceAttribute(), Target, false)
	AURA_ATTR_DEFINE_DEF(TargetArcaneResDef, GetArcaneResistanceAttribute(), Target, false)
	AURA_ATTR_DEFINE_DEF(TargetPhysicalResDef, GetPhysicalResistanceAttribute(), Target, false)

	AURA_ATTR_DEFINE_DEF(TargetHealthDef, GetHealthAttribute(), Target, false)

	/*FGameplayEffectAttributeCaptureDefinition SourceCritDamageDef, SourceArmorPenDef,
	TargetArmorDef, TargetBlockChanceDef, TargetCritHitResDef,
	TargetFireResDef, TargetLightningResDef, TargetArcaneResDef, TargetPhysicalResDef;*/
	const TArray<FGameplayEffectAttributeCaptureDefinition> DefArray = {
		SourceCritChanceDef, SourceCritDamageDef, SourceArmorPenDef,
		TargetArmorDef, TargetBlockChanceDef, TargetCritHitResDef,
		TargetFireResDef, TargetLightningResDef, TargetArcaneResDef, TargetPhysicalResDef,
		TargetHealthDef,
	};
	// FAuraDamageStatics() {/*DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);*/}
};
static const FAuraDamageStatics& DamageStatics() {static FAuraDamageStatics S; return S;}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Append(DamageStatics().DefArray);
}

// CDO->Execute(
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
// #if WITH_SERVER_CODE
	/* Boilerplate ~ */
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent(); // Get Form ExecutionParams.Spec
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	/* ~ End Boilerplate */

	const int32 SourcePlayerLevel = SourceAvatar ? Cast<AAuraCharacterBase>(SourceAvatar)->GetCharacterLevel() : 1;
	const int32 TargetPlayerLevel = TargetAvatar ? Cast<AAuraCharacterBase>(TargetAvatar)->GetCharacterLevel() : 1;

	// Get Damage Set by caller Magnitude
	float Damage = 0.f;
	auto DmgDebuffCalc = [&Spec, &ExecutionParams, &EvaluateParameters, &Damage]
		(const FGameplayTag& DamageType, const FGameplayEffectAttributeCaptureDefinition& ResistanceDef)
	{
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, 0.f);
		if (TypeDamage < UE_KINDA_SMALL_NUMBER) return false;
		float Resistance = 0.f; ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceDef, EvaluateParameters, Resistance);
		const float ResistedMultiplier = FMath::Max((100.f - Resistance) / 100.f, 0.f); // can't be negative (+HP)
		Damage += TypeDamage * ResistedMultiplier;
		return true;
	};
	DmgDebuffCalc(AuraTag::Damage_Fire, DamageStatics().TargetFireResDef) ?
	true : DmgDebuffCalc(AuraTag::Damage_Lightning, DamageStatics().TargetLightningResDef) ?
	true : DmgDebuffCalc(AuraTag::Damage_Arcane, DamageStatics().TargetArcaneResDef) ?
	true : DmgDebuffCalc(AuraTag::Damage_Physical, DamageStatics().TargetPhysicalResDef);

	auto GetAttributeMagnitude = [&](const FGameplayEffectAttributeCaptureDefinition& Def)
	{
		float Result = 0.f; ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Def, EvaluateParameters, Result);
		return Result;
	};
	auto GetAttributeMagnitudeClamped = [&ExecutionParams, &EvaluateParameters]
		(const FGameplayEffectAttributeCaptureDefinition& Def)
	{
		float Result = 0.f; ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(Def, EvaluateParameters, Result);
		return FMath::Max<float>(0.f, Result);
	};
	// Capture BlockChance and determine if there was a successful block
	const float TargetBlockChance = GetAttributeMagnitude(DamageStatics().TargetBlockChanceDef);
	//IMPORTANT: Spec remains for Period HasDuration GE, Context has to be reset
	/*FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(Spec.GetContext());*/

	FGameplayTagContainer CueTargetTags;
	if (FMath::FRandRange(0.f, 100.f) < TargetBlockChance)
	{
		CueTargetTags.AddTag(AuraTag::Damage_Blocked);
		Damage *= .5f;
	}

	const float TargetArmor = GetAttributeMagnitude(DamageStatics().TargetArmorDef);
	const float SourceArmorPenetration = GetAttributeMagnitudeClamped(DamageStatics().SourceArmorPenDef);

	// Get Graph
	const UCurveTable* DamageCalculationCurveTable =
		UCharacterClassDataAsset::GetFromGameMode(SourceAvatar)->DamageCalculationCoefficients;
	const FRealCurve* ArmorPenetrationCurve = DamageCalculationCurveTable->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
	const FRealCurve* EffectiveArmorCurve = DamageCalculationCurveTable->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);

	// Penetration ignores part of TargetAmor
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


	/** =================== Crit Calculation ======================================= */
	const float SourceCritChance = GetAttributeMagnitude(DamageStatics().SourceCritChanceDef);
	const float TargetCritRes = GetAttributeMagnitude(DamageStatics().TargetCritHitResDef);

	const FRealCurve* EffectiveCritResCurve = DamageCalculationCurveTable->FindCurve(FName("CriticalHitResistance"), FString());
	const float EffectiveCritResCoefficient = EffectiveCritResCurve->Eval(TargetPlayerLevel);

	const float EffectiveCritChance = FMath::Max(SourceCritChance - TargetCritRes * EffectiveCritResCoefficient, 0.f);
	if (FMath::RandRange(UE_SMALL_NUMBER, 100.f) < EffectiveCritChance)
	{
		CueTargetTags.AddTag(AuraTag::Damage_Crit);
		const float CritDmg = GetAttributeMagnitudeClamped(DamageStatics().SourceCritDamageDef);
		Damage *= (1 + CritDmg);
	}

	const float CurrentHealth = GetAttributeMagnitude(DamageStatics().TargetHealthDef);
	const float DmgHealthDiff = Damage - CurrentHealth;
	if (DmgHealthDiff > 0)
	{	// Send Event to trigger any ability that activates on damaging other (source) or damaged (target)
		FGameplayEventData Data;
		// SourceASC->HandleGameplayEvent()
		// TargetASC->HandleGameplayEvent();
	}

	AURA_ADD_OUTPUT_MODIFIER(GetIncomingDamageAttribute(), Override, Damage)
	// if IncomingDamageProperty is added with DECLARE_ATTRIBUTE_CAPTUREDEF and DEFINE_ATTRIBUTE_CAPTUREDEF.
	/*OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		DamageStatics().IncomingDamageProperty, EGameplayModOp::Override, Damage));*/

	// Educational Purpose
	// AURA_ADD_OUTPUT_MODIFIER(GetManaAttribute(), AddBase, -2.f)

	if (Damage < UE_SMALL_NUMBER || CueTargetTags.Num() > 0)
	{
		OutExecutionOutput.MarkGameplayCuesHandledManually();
		FGameplayCueParameters CueParams;
		UAbilitySystemGlobals::Get().InitGameplayCueParameters_GESpec(CueParams, Spec);
		CueParams.RawMagnitude = Damage;
		CueParams.AggregatedTargetTags.AppendTags(CueTargetTags);
		if (Spec.Period > 0.f)
		{
			CueParams.Location = TargetAvatar->GetActorLocation();
		}
		TargetASC->ExecuteGameplayCue(AuraTag::GameplayCue_Shared_Damage, CueParams);
	}
}
