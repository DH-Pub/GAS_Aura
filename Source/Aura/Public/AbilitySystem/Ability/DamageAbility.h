// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CostCooldownAbility.h"
#include "DamageAbility.generated.h"

USTRUCT(BlueprintType) // USTRUCT needed for FInstancedStruct::Make()
struct FDamageEffectContext
{
	GENERATED_BODY()

	FDamageEffectContext() {}
	explicit FDamageEffectContext(AActor* TargetActor) : TargetActor(TargetActor) {}

	UPROPERTY(/* Need this to replicate data to client */)
	FVector_NetQuantize DamageDirection = FVector::ZeroVector;

	void ResetForDebuff()
	{
		bIsBlocked = bIsCrit = false;
		TargetActor = nullptr;
	}
	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocked = false; bool SetIsBlocked(const bool bIn) {return bIsBlocked = bIn;}
	UPROPERTY(BlueprintReadOnly)
	bool bIsCrit = false; bool SetIsCrit(const bool bIn) {return bIsCrit = bIn;}
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY()
	TObjectPtr<USoundBase> ImpactSound = nullptr;
	UPROPERTY()
	TObjectPtr<class UNiagaraSystem> ImpactEffect = nullptr;
};
/**
 * Ability that can apply damage effect
 * Can also apply debuff
 */
UCLASS()
class AURA_API UDamageAbility : public UCostCooldownAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Dmg", DefaultToSelf="Ability", HidePin="Ability"))
	static float GetDamageAtLevel(const UDamageAbility* Ability, const int32 Level)
	{return static_cast<int32>(Ability->DamageValue.GetValueAtLevel(Level) * 10.f) / 10.f;}

	UPROPERTY(EditDefaultsOnly, Category="Default|Damage")
	bool bStagger = false;

	UPROPERTY(EditDefaultsOnly, Category="Default|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	UPROPERTY(EditDefaultsOnly, Category="Default|Damage", meta=(ForceInlineRow, GameplayTagFilter="Damage"))
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, Category="Default|Damage")
	FScalableFloat DamageValue;
	FGameplayEffectSpecHandle MakeDamageSpecHandle() const;

	UPROPERTY(EditDefaultsOnly, Category="Default|Damage", meta=(UIMin=0.01, UIMax=10.0, Delta=0.01))
	float DebuffDelay = 0.2; // Delay until debuff activates
	UPROPERTY(EditDefaultsOnly, Category="Default|Debuff")
	TSubclassOf<UGameplayEffect> DebuffEffectClass;
	UPROPERTY(EditDefaultsOnly, Category="Default|Debuff", meta=(GameplayTagFilter="Debuff.Type"))
	FGameplayTag DebuffType = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, Category="Default|Debuff", meta=(UIMin=0, UIMax=1 /*, ShowOnlyInnerProperties*/))
	FScalableFloat DebuffChance = .2f;
	UPROPERTY(EditDefaultsOnly, Category="Default|Debuff")
	FScalableFloat DebuffDamage = .5f;
	UPROPERTY(EditDefaultsOnly, Category="Default|Debuff")
	FScalableFloat DebuffDuration = 4.f;
	UPROPERTY(EditDefaultsOnly, Category="Default|Debuff")
	FScalableFloat DebuffPeriod = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Default|Damage")
	float KnockbackForce = 200.f;
	UPROPERTY(EditDefaultsOnly, Category="Default|Damage")
	float KnockbackChance = 0.f;
	UPROPERTY(EditDefaultsOnly, Category="Default|Damage")
	float DeathImpulseMagnitude = 600.f; // Knockback on death
protected:
	UFUNCTION(BlueprintCallable)
	void CauseDamageToActors(const TArray<AActor*>& Actors, USoundBase* ImpactSound);
	// Call in GC_Damage
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool ExecuteCueShowDamage(const FGameplayCueParameters& Parameters, float& OutDamage,
		FDamageEffectContext& OutDamageContext, FVector& OutLoc);
};


UCLASS()
class AURA_API UDamageGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UDamageGameplayEffect();
};


UCLASS()
class AURA_API UDebuffDamageEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UDebuffDamageEffect();
};
