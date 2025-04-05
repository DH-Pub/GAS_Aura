// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassDataAsset.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	DefaultClass, // if you want to mix multiple classes
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
};

/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="PrimaryAttributes", ForceInlineRow))
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	// Attributes shared for all classes (all will have the same secondary and vital)
	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	UPROPERTY(EditDefaultsOnly, Category = "CommonClassDefaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	FORCEINLINE FCharacterClassDefaultInfo GetClassDefaultInfo(const ECharacterClass CharacterClass) {return CharacterClassInformation.FindChecked(CharacterClass);}
};
