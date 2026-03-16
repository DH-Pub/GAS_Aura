// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "AttributesEventAbility.generated.h"

USTRUCT()
struct FAttributeData
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag Tag = FGameplayTag::EmptyTag;
	UPROPERTY()
	int32 AttributeMagnitude = 0;

	bool operator==(const FAttributeData& Other) const
	{
		return Tag.MatchesTagExact(Other.Tag) && AttributeMagnitude == Other.AttributeMagnitude;
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Tag.NetSerialize_Packed(Ar, Map, bOutSuccess);
		Ar << AttributeMagnitude;
		return bOutSuccess = true;
	}
};

USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_AttributeData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()
	FGameplayAbilityTargetData_AttributeData() {}

	UPROPERTY()
	TArray<FAttributeData> Data;

	void AddNewData(const FGameplayTag& Tag, const int32 Points)
	{
		if (!Tag.IsValid() || FindPointsPtr(Tag)) return; // Already has, modify existing instead
		Data.Add(FAttributeData(Tag, Points));
	}

	int32* FindPointsPtr(const FGameplayTag& InTag)
	{
		for (auto& [Tag, Mag] : Data) {if (Tag.MatchesTagExact(InTag)) return &Mag;}
		return nullptr;
	}
	int32 TotalPointsAllocating() const
	{
		int32 TotalPoints = 0;
		for (auto& [Tag, Mag] : Data) TotalPoints += Mag;
		return TotalPoints;
	}

	// Required for all child structs of FGameplayAbilityTargetData
	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct();}
	// Required for all child structs of FGameplayAbilityTargetData
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		return bOutSuccess |= SafeNetSerializeTArray_WithNetSerialize<31>(Ar, Data, Map);
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
 * - ::SendGameplayEventToActor -> ASC->HandleGameplayEvent with tag Attributes to activate this ability
 * - if Activate in BP, make it passive and use [Wait Gameplay Event] with EventTag
 */
UCLASS()
class AURA_API UAttributesEventAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UAttributesEventAbility();
protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
private:
	UPROPERTY()
	TObjectPtr<class AAuraPlayerState> AuraPS;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Attribute")
	TSubclassOf<UGameplayEffect> AttributeEffect;

	/* Separate GE because there were issue "CurrentModcallbackData was not consumed For attribute" */
	UPROPERTY(EditDefaultsOnly, Category="Aura|Attribute")
	TSubclassOf<UGameplayEffect> XPEffect;
};
