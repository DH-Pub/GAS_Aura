// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" // GAMEPLAYATTRIBUTE_REPNOTIFY , ATTRIBUTE_ACCESSORS
#include "AuraAttributeSet.generated.h"

#define AURA_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// For Init FGameplayEffectAttributeCaptureDefinition
#define AURA_DEFINE_CAPTURE_DEF(Def, Property, T, bSnapShot)\
Def = FGameplayEffectAttributeCaptureDefinition(UAuraAttributeSet::Property, EGameplayEffectAttributeCaptureSource::T, bSnapShot);
// FOr UExecCalc_ Execute_Implementation(), to add modifier
#define AURA_ADD_OUTPUT_MODIFIER(Property, Op, Value)\
OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UAuraAttributeSet::Property, EGameplayModOp::Op, Value));

/**
 * Used in UGameplayEffect() Constructor
 * Create [FGameplayModifierInfo Info;] [FAttributeBasedFloat AttributeBasedFloat;] before using this
 */
#define AURA_ADD_ATTRIBUTE_BASED_MODIFIER(Property, Op, Mul, PreAdd, PoseAdd, Capture)\
{\
	Info.Attribute = UAuraAttributeSet::Property;\
	Info.ModifierOp = EGameplayModOp::Op;\
	AttributeBasedFloat.Coefficient = Mul;\
	AttributeBasedFloat.PreMultiplyAdditiveValue = PreAdd;\
	AttributeBasedFloat.PostMultiplyAdditiveValue = PoseAdd;\
	AURA_DEFINE_CAPTURE_DEF(AttributeBasedFloat.BackingAttribute, Capture, Target, false)\
	Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(AttributeBasedFloat);\
	Modifiers.Add(Info);\
}
/**
 * Used in UGameplayEffect() Constructor
 * Create [FGameplayModifierInfo Info;] [FCustomCalculationBasedFloat CustomCalculation;] before using this
 */
#define AURA_ADD_CUSTOM_CALCULATION_MODIFIER(Property, Op, CalcClass)\
{\
	Info.Attribute = UAuraAttributeSet::Property;\
	Info.ModifierOp = EGameplayModOp::Op;\
	CustomCalculation.CalculationClassMagnitude = CalcClass::StaticClass();\
	Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(CustomCalculation);\
	Modifiers.Add(Info);\
}

class AAuraCharacterBase;

/*USTRUCT()
struct FEffectActor // NOT in use
{
	GENERATED_BODY()
	
	UPROPERTY(Transient)
	UAbilitySystemComponent* AbilitySystemComponent = nullptr;
	UPROPERTY(Transient)
	AActor* AvatarActor = nullptr;
	UPROPERTY(Transient)
	AController* Controller = nullptr;
	UPROPERTY(Transient)
	AAuraCharacterBase* Character = nullptr;
};*/

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	
	FEffectProperties()
	{
		// Source = Target = MakeShared<FEffectActor>();
	}
	explicit FEffectProperties(const FGameplayEffectModCallbackData& Data);
	
	// TSharedPtr<FEffectActor> Source; TSharedPtr<FEffectActor> Target;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;
	UPROPERTY()
	TObjectPtr<AController> SourceController = nullptr;
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> SourceCharacter = nullptr;
	
	UPROPERTY()
	TObjectPtr<AController> TargetController = nullptr;
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> TargetCharacter = nullptr; // AvatarActor cast into AuraCharacterBase
};

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
private:
	void HandleIncomingDamage(const FGameplayEffectModCallbackData& Data, const FEffectProperties& Props);
	void HandleIncomingXP(const FGameplayEffectModCallbackData& Data, const FEffectProperties& Props);

public:
#pragma region PrimaryAttributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category="Attributes|Primary")
	FGameplayAttributeData Strength;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength)
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category="Attributes|Primary")
	FGameplayAttributeData Intelligence;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence)
	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category="Attributes|Primary")
	FGameplayAttributeData Resilience;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience)
	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Vigor, Category="Attributes|Primary")
	FGameplayAttributeData Vigor;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor)
	UFUNCTION()
	void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);}
#pragma endregion


#pragma region SecondaryAttributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category="Attributes|Secondary")
	FGameplayAttributeData Armor;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor)
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category="Attributes|Secondary")
	FGameplayAttributeData ArmorPenetration;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration)
	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category="Attributes|Secondary")
	FGameplayAttributeData BlockChance;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance)
	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category="Attributes|Secondary")
	FGameplayAttributeData CriticalHitChance;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance)
	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category="Attributes|Secondary")
	FGameplayAttributeData CriticalHitDamage;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage)
	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category="Attributes|Secondary")
	FGameplayAttributeData CriticalHitResistance;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance)
	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category="Attributes|Secondary")
	FGameplayAttributeData HealthRegeneration;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration)
	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category="Attributes|Secondary")
	FGameplayAttributeData ManaRegeneration;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration)
	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category="Attributes|Secondary")
	FGameplayAttributeData MaxHealth;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth)
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category="Attributes|Secondary")
	FGameplayAttributeData MaxMana;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana)
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;
#pragma endregion


#pragma region ResistanceAttributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category="Attributes|Resistance")
	FGameplayAttributeData FireResistance;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, FireResistance)
	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireRes) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireRes);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightningResistance, Category="Attributes|Resistance")
	FGameplayAttributeData LightningResistance;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, LightningResistance)
	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneResistance, Category="Attributes|Resistance")
	FGameplayAttributeData ArcaneResistance;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArcaneResistance)
	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category="Attributes|Resistance")
	FGameplayAttributeData PhysicalResistance;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, PhysicalResistance)
	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);}
#pragma endregion


#pragma region VitalAttributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category="Attributes|Vital")
	FGameplayAttributeData Health;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health)
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category="Attributes|Vital")
	FGameplayAttributeData Mana;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana)
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category="Attributes|Vital")
	FGameplayAttributeData MovementSpeed;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MovementSpeed)
	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldSpeed) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MovementSpeed, OldSpeed);}
#pragma endregion


#pragma region MetaAttributes
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_IncomingDamage, Category="Attributes|Meta")
	FGameplayAttributeData IncomingDamage;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage)
	UFUNCTION() void OnRep_IncomingDamage(const FGameplayAttributeData& OldDamage) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, IncomingDamage, OldDamage);}
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_IncomingXP, Category="Attributes|Meta")
	FGameplayAttributeData IncomingXP;
	AURA_ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingXP)
	UFUNCTION() void OnRep_IncomingXP(const FGameplayAttributeData& OldXP) const
	{GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, IncomingXP, OldXP);}
#pragma endregion
};
