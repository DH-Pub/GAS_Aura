// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AttributesEventAbility.generated.h"

USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_AttributeData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGameplayAbilityTargetData_AttributeData() {}

	UPROPERTY()
	TArray<FGameplayTag> AttributeTags;
	UPROPERTY()
	TArray<int32> AttributeMagnitudes;

	/** Returns all actors targeted, almost always overridden */
	virtual TArray<TWeakObjectPtr<AActor>> GetActors() const
	{
		return TArray<TWeakObjectPtr<AActor>>();
	}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		SafeNetSerializeTArray_Default<31>(Ar, AttributeTags);
		SafeNetSerializeTArray_Default<31>(Ar, AttributeMagnitudes);
		bOutSuccess = true;
		return true;
	}
};
template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_AttributeData> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_AttributeData>
{
	enum
	{
		WithNetSerializer = true // This is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
	};
};

/**
 * - Receive event to upgrade Attributes (XP, Strength, ...)
 * - UAbilitySystemBlueprintLibrary::SendGameplayEventToActor with tag Attributes to activate this ability
 * - if Activate in BP, make it passive and use [Wait Gameplay Event] with EventTag
 */
UCLASS()
class AURA_API UAttributesEventAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UAttributesEventAbility();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};


/**
 * Increase/Decrease Attributes
 */
UCLASS()
class AURA_API UAttributeEventEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UAttributeEventEffect();
};
