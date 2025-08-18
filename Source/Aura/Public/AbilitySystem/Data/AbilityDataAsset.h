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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(GameplayTagFilter="Ability"))
	FGameplayTag CooldownTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag(); // Set in C++
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag(); // Set in C++
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAuraGameplayAbility> AbilityClass = nullptr;

	void SetInputAndStatusTag(const FGameplayAbilitySpec& AbilitySpec);
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

	FAuraAbilityData* FindAbilityDataByTag(const FGameplayTagContainer& AbilityTags);

	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
