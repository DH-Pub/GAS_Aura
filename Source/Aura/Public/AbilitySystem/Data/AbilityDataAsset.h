// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

struct FGameplayAbilitySpecHandle;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAuraAbilityData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(GameplayTagFilter="Abilities"))
	FGameplayTag AbilityTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(GameplayTagFilter="Ability"))
	FGameplayTag CooldownTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;
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
};
