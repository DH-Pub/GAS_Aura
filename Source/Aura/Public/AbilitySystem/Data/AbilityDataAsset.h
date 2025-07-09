// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAuraAbilityDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(GameplayTagFilter="Abilities"))
	FGameplayTag AbilityTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(GameplayTagFilter="Cooldown"))
	FGameplayTag CooldownTag = FGameplayTag();
	UPROPERTY(BlueprintReadOnly, meta=(GameplayTagFilter="Input"))
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UMaterialInterface> BackgroundMaterial = nullptr;
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
	TArray<FAuraAbilityDataAsset> AbilityData;

	UFUNCTION(BlueprintCallable)
	FAuraAbilityDataAsset FindAbilityDataByTag(const FGameplayTagContainer& AbilityTags, bool bLogNotFound = false);
};
