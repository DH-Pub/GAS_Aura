// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

struct FGameplayAbilitySpec;
class UAuraAbilitySystemComponent;
class UAuraGameplayAbility;

USTRUCT(BlueprintType)
struct FAuraAbilityData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(GameplayTagFilter="Ability"))
	FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsPassive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAuraGameplayAbility> AbilityClass = nullptr;
};
//Data Asset is shared among players, this is created to send current player's ability data
USTRUCT(BlueprintType)
struct FPlayerAbilityData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class AURA_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	/** Data Asset list of all abilities with icons, ... */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="{AbilityTag} - {Icon}"), Category="AbilityInformation")
	TArray<FAuraAbilityData> AbilityDataList;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;

	UFUNCTION(CallInEditor, Category="Populate Data")
	void PopulateDataAsset();
#endif
};
