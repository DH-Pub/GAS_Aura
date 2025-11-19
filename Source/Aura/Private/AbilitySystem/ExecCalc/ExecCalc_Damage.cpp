// Copyright Hung


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
// #include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "AuraEffectTypes.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "Character/AuraCharacterBase.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	FGameplayEffectAttributeCaptureDefinition SourceCritDamageDef; // Alternative Way
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor); // Create ArmorDef and ArmorProperty
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		AURA_DEFINE_CAPTURE_DEF(SourceCritDamageDef, GetCriticalHitDamageAttribute(), Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
	}
};
static const AuraDamageStatics& DamageStatics() {static AuraDamageStatics S;return S;}
UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().SourceCritDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);

	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

// CDO->Execute(
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	/* Boilerplate ~ */
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent(); // Get Form ExecutionParams.Spec
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
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false);
		if (TypeDamage < UE_KINDA_SMALL_NUMBER) return false;
		float Resistance = 0.f; ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceDef, EvaluateParameters, Resistance);
		const float ResistedMultiplier = (100.f - Resistance) / 100.f;
		Damage += TypeDamage * ResistedMultiplier;

		/*// Debuff
		const UDamageAbility* DamageAbility = Cast<UDamageAbility>(Spec.GetContext().GetAbility());
		const float SourceDebuffChance = DamageAbility->DebuffChance.GetValueAtLevel(Spec.GetLevel());
		DamageContext.SetDebuffSuccess(FMath::RandRange(0.f, 1.f) < SourceDebuffChance * ResistedMultiplier);*/
		return true;
	};
	DmgDebuffCalc(AuraGameplayTags::Damage_Fire, DamageStatics().FireResistanceDef) ?
	true : DmgDebuffCalc(AuraGameplayTags::Damage_Lightning, DamageStatics().LightningResistanceDef) ?
	true : DmgDebuffCalc(AuraGameplayTags::Damage_Arcane, DamageStatics().ArcaneResistanceDef) ?
	true : DmgDebuffCalc(AuraGameplayTags::Damage_Physical, DamageStatics().PhysicalResistanceDef);

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
	const float TargetBlockChance = GetAttributeMagnitude(DamageStatics().BlockChanceDef);
	/*//IMPORTANT: Spec remains for Period HasDuration GE, so Context needs to be reset or changed to current
	FAuraEffectContext* AuraContext = FAuraEffectContext::GetAuraContext(Spec.GetContext().Get());
	AuraContext.SetIsBlocked(FMath::FRandRange(0.f, 100.f) < TargetBlockChance));*/
	FDamageEffectContext* DamageContext = FAuraEffectContext::GetOrMakeContextStructPtr<FDamageEffectContext>(Spec.GetContext());
	if (DamageContext->SetIsBlocked(FMath::FRandRange(0.f, 100.f) < TargetBlockChance)) Damage *= .5f;

	const float TargetArmor = GetAttributeMagnitude(DamageStatics().ArmorDef);
	const float SourceArmorPenetration = GetAttributeMagnitudeClamped(DamageStatics().ArmorPenetrationDef);

	// Get Graph
	const UCurveTable* DamageCalculationCurveTable = UCharacterClassDataAsset::GetFromGameMode(SourceAvatar)
		->DamageCalculationCoefficients;
	const FRealCurve* ArmorPenetrationCurve = DamageCalculationCurveTable->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);
	const FRealCurve* EffectiveArmorCurve = DamageCalculationCurveTable->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);

	// Penetration ignores part of TargetAmor
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


	/** =================== Crit Calculation ======================================= */
	const float SourceCritChance = GetAttributeMagnitude(DamageStatics().CriticalHitChanceDef);
	const float TargetCritRes = GetAttributeMagnitude(DamageStatics().CriticalHitResistanceDef);

	const FRealCurve* EffectiveCritResCurve = DamageCalculationCurveTable->FindCurve(FName("CriticalHitResistance"), FString());
	const float EffectiveCritResCoefficient = EffectiveCritResCurve->Eval(TargetPlayerLevel);

	const float EffectiveCritChance = SourceCritChance - TargetCritRes * EffectiveCritResCoefficient;
	if (DamageContext->SetIsCrit(FMath::RandRange(0.f, 100.f) <= EffectiveCritChance))
	{
		const float CritDmg = GetAttributeMagnitudeClamped(DamageStatics().SourceCritDamageDef);
		Damage *= (1 + CritDmg);
	}

	// DamageStatics().ArmorProperty FProperty* used to briefly hold the attribute
	// This will override previous IncomingDamage
	/*OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		DamageStatics().IncomingDamageProperty, EGameplayModOp::Override, Damage)); // if IncomingDamage is added*/
	/*OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, Damage));*/
	AURA_ADD_OUTPUT_MODIFIER(GetIncomingDamageAttribute(), Override, Damage)

	// Educational Purpose
	AURA_ADD_OUTPUT_MODIFIER(GetManaAttribute(), Additive, -2.f)
}
