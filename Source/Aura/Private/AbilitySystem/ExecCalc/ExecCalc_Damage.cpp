// Copyright Hung


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayEffectTypes.h"
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
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);
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
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
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
	float Damage = 0.f;
	auto DmgAfterResistance = [&Spec, &ExecutionParams, &EvaluateParameters, &Damage](const FGameplayTag& DamageTypeTag,
		const FGameplayEffectAttributeCaptureDefinition& CaptureDef)
	{
		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false);
		if (DamageTypeValue == 0.f) return;
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
		Damage += DamageTypeValue * (100.f - Resistance) / 100.f;
	};
	DmgAfterResistance(AuraGameplayTags::Damage_Fire, DamageStatics().FireResistanceDef);
	DmgAfterResistance(AuraGameplayTags::Damage_Lightning, DamageStatics().LightningResistanceDef);
	DmgAfterResistance(AuraGameplayTags::Damage_Arcane, DamageStatics().ArcaneResistanceDef);
	DmgAfterResistance(AuraGameplayTags::Damage_Physical, DamageStatics().PhysicalResistanceDef);


	auto GetAttributeMagnitudeClamped = [&ExecutionParams, &EvaluateParameters] (const FGameplayEffectAttributeCaptureDefinition& CaptureDef, float& Magnitude)
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Magnitude);
		Magnitude = FMath::Max<float>(0.f, Magnitude);
	};
	// Capture BlockChance and determine if there was a successful block
	float TargetBlockChance = 0.f;
	GetAttributeMagnitudeClamped(DamageStatics().BlockChanceDef, TargetBlockChance);
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	if (FMath::FRandRange(UE_KINDA_SMALL_NUMBER, 100.f) <= TargetBlockChance)
	{
		UAuraAbilitySystemLibrary::SetIsBlocked(EffectContextHandle, true);
		Damage *= .5f;
	}
	
	float TargetArmor = 0.f;
	GetAttributeMagnitudeClamped(DamageStatics().ArmorDef, TargetArmor);
	float SourceArmorPenetration = 0.f;
	GetAttributeMagnitudeClamped(DamageStatics().ArmorPenetrationDef, SourceArmorPenetration);

	// Get Graph
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
	GetAttributeMagnitudeClamped(DamageStatics().CriticalHitChanceDef, SourceCritChance);
	float TargetCritRes = 0.f;
	GetAttributeMagnitudeClamped(DamageStatics().CriticalHitResistanceDef, TargetCritRes);
	
	FRealCurve* EffectiveCritResCurve = DamageCalculationCurveTable->FindCurve(FName("CriticalHitResistance"), FString());
	const float EffectiveCritResCoefficient = EffectiveCritResCurve->Eval(TargetCombatInterface->GetCharacterLevel());
	
	const float EffectiveCritChance = SourceCritChance - TargetCritRes * EffectiveCritResCoefficient;
	if (FMath::RandRange(UE_KINDA_SMALL_NUMBER, 100.f) <= EffectiveCritChance)
	{
		float SourceCritDamage = 0.f;
		GetAttributeMagnitudeClamped(DamageStatics().CriticalHitDamageDef, SourceCritDamage);
		UAuraAbilitySystemLibrary::SetIsCrit(EffectContextHandle, true);
		Damage *= (1 + SourceCritDamage);
	}

	
	// DamageStatics().ArmorProperty FProperty* used to briefly hold the attribute
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Override, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
