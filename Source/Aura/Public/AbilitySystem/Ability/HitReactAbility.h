// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "HitReactAbility.generated.h"

namespace EDrawDebugTrace {enum Type : int;}

USTRUCT(BlueprintType)
struct FGATargetData_HitReact : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGATargetData_HitReact(){}

	UPROPERTY(EditDefaultsOnly)
	int32 KnockbackDistance = 0;
	UPROPERTY(EditDefaultsOnly)
	float KnockbackTime = 0.2f;
	UPROPERTY()
	FVector_NetQuantizeNormal KnockbackDirection; // can be vertical or horizontal

	UPROPERTY()
	FGameplayEffectSpecHandle HitWallEffectSpecHandle;

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override {return StaticStruct();}
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << KnockbackDistance;
		Ar << KnockbackTime;
		bOutSuccess |= KnockbackDirection.NetSerialize(Ar, Map, bOutSuccess);
		if (HitWallEffectSpecHandle.IsValid())
		{
			bOutSuccess |= HitWallEffectSpecHandle.NetSerialize(Ar, Map, bOutSuccess);
		}
		return bOutSuccess;
	}
};
template<>
struct TStructOpsTypeTraits<FGATargetData_HitReact> : public TStructOpsTypeTraitsBase2<FGATargetData_HitReact>
{	// This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	enum {WithNetSerializer = true};
};

UCLASS()
class AURA_API UHitReactAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UHitReactAbility();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHitReact();
	UFUNCTION(BlueprintImplementableEvent)
	void Knockback(FVector ToLocation, const float InDuration = 0.f, const bool bInterrupted = false);

	UPROPERTY(EditDefaultsOnly, Category="Aura|Knockback")
	TEnumAsByte<EDrawDebugTrace::Type> KnockbackDebug;

	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer HitReactTags;

	UFUNCTION(BlueprintCallable)
	void TryEndHitReact(const FGameplayTag Tag);
};
