// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassDataAsset.generated.h"

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
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	// Attributes shared for all classes (all will have the same because they are based on PrimaryAttributes)
	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients; 
	
	FORCEINLINE FCharacterClassDefaultInfo GetClassDefaultInfo(const ECharacterClass CharacterClass)
	{
		return CharacterClassInformation.FindChecked(CharacterClass);
	}
};
