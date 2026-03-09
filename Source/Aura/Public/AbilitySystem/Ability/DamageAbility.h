// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CostCooldownAbility.h"
#include "HitReactAbility.h"
#include "DamageAbility.generated.h"

USTRUCT(BlueprintType)
struct FAbilityEffect
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EffectClass;
	UPROPERTY(EditDefaultsOnly)
	float Period = 0.f;
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, FScalableFloat> SetByCallerTagMagnitudes;
};

/**
 * Ability that can apply damage effect
 * Can also apply debuff
 */
UCLASS()
class AURA_API UDamageAbility : public UCostCooldownAbility //TODO: Rename to EffectAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TArray<FAbilityEffect> AbilityEffects;
	TArray<FGameplayEffectSpecHandle> MakeOutgoingAbilityEffectsSpecs();
	virtual void ApplyAbilityEffectsToTarget(const AActor* InTarget, TArray<FGameplayEffectSpecHandle>& SpecHandles,
		const FVector& InDirection); //TODO: Make struct for additional info (Direction, ...)

	UPROPERTY(EditDefaultsOnly, Category="Aura|HitReact", meta=(GameplayTagFilter="State.HitReact."))
	FGameplayTagContainer HitReactTargetTags;
	UPROPERTY(EditDefaultsOnly, Category="Aura|HitReact", meta=(UIMin=0, UIMax=1, ClampMin=0, ClampMax=1, Delta=0.01))
	float KnockbackChance = 0.f; // Not used for periodical effect
	UPROPERTY(EditDefaultsOnly, Category="Aura|HitReact", meta=(EditCondition="KnockbackChance > 0", EditConditionHides))
	FGATargetData_HitReact HitReactTargetData;
	UPROPERTY(EditDefaultsOnly, Category="Aura|HitReact", meta=(EditCondition="KnockbackChance > 0", EditConditionHides))
	FAbilityEffect HitWallEffect; // On knockback hit wall
	UPROPERTY(EditDefaultsOnly, Category="Aura|Damage")
	float DeathImpulseMagnitude = 600.f; // Knockback on death
protected:
	UFUNCTION(BlueprintCallable)
	void MeleeTraceAndApplyEffects(const float Radius, const FVector& InLoc, const FGameplayTag& ImpactCue,
		EDrawDebugTrace::Type DrawDebugType);

public:
	// Call in GC_Damage, Return Value will decide whether to show Damage Number
	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"))
	static bool ExecuteCueShowDamage(const FGameplayCueParameters& Parameters, bool& bBlocked, bool& bCrit);
	virtual void GetAbilityDetails(FAbilityDetails& Details) const override;
};
