// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"
#include "CharacterClassDataAsset.generated.h"

class AAuraCharacterBase;
class UAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	DefaultClass, // if you want to mix multiple classes
	Aura,
	Elementalist,
	Warrior,
	Ranger,
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "ClassDefaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "ClassDefaults")
	TArray<TSubclassOf<UGameplayAbility>> ClassAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "ClassDefaults")
	FScalableFloat XPReward = 0.f;
};

/**
 *
 */
UCLASS()
class AURA_API UCharacterClassDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="{PrimaryAttributes}", ForceInlineRow))
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	// Attributes shared for all classes (all will have the same because they are based on PrimaryAttributes)
	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TArray<TSubclassOf<class UAuraGameplayAbility>> CommonAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	/**
	 * Get Server DA_CharacterClass, can't be changed -> const
	 * Client can't access GameMode so this will always return nullptr
	 */
	static const UCharacterClassDataAsset* GetFromGameMode(const UObject* WorldContextObject);

	void InitializeDefaultAttributes(const ECharacterClass CharacterClass, const float Level,
		UAbilitySystemComponent* ASC) const;
	void GiveStartupAbilities(const AAuraCharacterBase* AuraCharacter) const;
	void SendXPToDeathCauser(UAbilitySystemComponent* Causer, const AAuraCharacterBase* DeadCharacter) const;

protected:
	const FCharacterClassDefaultInfo* GetClassDefaultInfo(const ECharacterClass CharacterClass) const
	{return CharacterClassInformation.Find(CharacterClass);}
};
