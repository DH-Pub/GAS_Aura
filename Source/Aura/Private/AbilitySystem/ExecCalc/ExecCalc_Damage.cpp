// Copyright Hung


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor); // Create ArmorDef
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	
	// SourceBlockChanceDef = FGameplayEffectAttributeCaptureDefinition(UAuraAttributeSet::GetBlockChanceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
	// TargetBlockChanceDef = FGameplayEffectAttributeCaptureDefinition(UAuraAttributeSet::GetBlockChanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	TScriptInterface<ICombatInterface> SourceCombatInterface = SourceAvatar;
	TScriptInterface<ICombatInterface> TargetCombatInterface = TargetAvatar;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Get Damage Set by caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(AuraGameplayTags::Damage_Incoming);
	
	// Capture BlockChance and determine if there was a successful block
	float TargetBlockChance = 0.f;
	GetAttributeMagnitude(ExecutionParams, EvaluateParameters, DamageStatics().BlockChanceDef, TargetBlockChance);
	const bool bBlocked = FMath::FRandRange(UE_KINDA_SMALL_NUMBER, 100.f) <= TargetBlockChance;
	if (bBlocked) Damage *= .5f;
	
	float TargetArmor = 0.f;
	GetAttributeMagnitude(ExecutionParams, EvaluateParameters, DamageStatics().ArmorDef, TargetArmor);
	float SourceArmorPenetration = 0.f;
	GetAttributeMagnitude(ExecutionParams, EvaluateParameters, DamageStatics().ArmorPenetrationDef, SourceArmorPenetration);

	TObjectPtr<UCurveTable> DamageCalculationCurveTable = UAuraAbilitySystemLibrary::GetCharacterClassDataAsset(SourceAvatar)->DamageCalculationCoefficients;
	FRealCurve* ArmorPenetrationCurve = DamageCalculationCurveTable->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetCharacterLevel());
	FRealCurve* EffectiveArmorCurve = DamageCalculationCurveTable->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetCharacterLevel());
	
	// Penetration ignores part of TargetAmor
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	/** =================== Crit Calculation ======================================= */
	float SourceCritChance = 0.f;
	GetAttributeMagnitude(ExecutionParams, EvaluateParameters, DamageStatics().CriticalHitChanceDef, SourceCritChance);
	float SourceCritDamage = 0.f;
	GetAttributeMagnitude(ExecutionParams, EvaluateParameters, DamageStatics().CriticalHitDamageDef, SourceCritDamage);
	float TargetCritRes = 0.f;
	GetAttributeMagnitude(ExecutionParams, EvaluateParameters, DamageStatics().CriticalHitResistanceDef, TargetCritRes);
	
	FRealCurve* EffectiveCritResCurve = DamageCalculationCurveTable->FindCurve(FName("CriticalHitResistance"), FString());
	const float EffectiveCritResCoefficient = EffectiveCritResCurve->Eval(TargetCombatInterface->GetCharacterLevel());
	
	const float EffectiveCritChance = SourceCritChance - TargetCritRes * EffectiveCritResCoefficient;
	const bool bCrit = FMath::RandRange(UE_KINDA_SMALL_NUMBER, 100.f) <= EffectiveCritChance;
	if (bCrit) Damage *= (1 + SourceCritDamage);
	
	// DamageStatics().ArmorProperty FProperty* used to briefly hold the attribute
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}


void UExecCalc_Damage::GetAttributeMagnitude(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	const FAggregatorEvaluateParameters& EvaluateParameters, const FGameplayEffectAttributeCaptureDefinition& AttributeDef, float& Attribute) const
{
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AttributeDef, EvaluateParameters, Attribute);
	Attribute = FMath::Max<float>(0.f, Attribute);
}