// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAuraAbilityData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UAuraGameplayAbility> AbilityClass = nullptr;
	const FGameplayTag& GetAuraAbilityTag() const;
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
	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty="{AbilityTag} - {Icon}"), Category="AbilityInformation")
	TArray<FAuraAbilityData> AbilityDataList;
	
	static const UAbilityDataAsset* GetFromGameState(const UObject* WorldContextObject);
	static const FAuraAbilityData* GetAbilityFromGameState(const UObject* WorldContextObject, const FGameplayTag& Tag);
	static const FAuraAbilityData* GetAbilityFromGameState(const UObject* WorldContextObject, const FGameplayTagContainer& Tags);
	static const FAuraAbilityData* GetAbilityFromGameState(const UObject* WorldContextObject,
		const TSubclassOf<UAuraGameplayAbility> AbilityClass);
	
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject", HidePin="WorldContextObject"))
	static const FGameplayTag& GetAbilityTagFromClass(const UObject* WorldContextObject,
		const TSubclassOf<UAuraGameplayAbility> AbilityClass);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	/*UFUNCTION(CallInEditor, Category="Populate Data")
	void PopulateDataAsset();*/
#endif
};
