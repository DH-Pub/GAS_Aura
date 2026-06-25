// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAuraAbilityData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UAuraGameplayAbility> AbilityClass = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Details")
	FText AbilityName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Details", meta=(DisplayThumbnail="true"))
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Details")
	TObjectPtr<UMaterialInterface> BackgroundMaterial = nullptr;


	//TODO: Create Separate DataAsset for abilities unlocked by each characters and their unlock/given conditions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1; // Give Ability at this Level
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoUnlock = false; // whether to auto unlock and give ability

	UPROPERTY(EditDefaultsOnly)
	float OrderIndex = 0.f; // For Sorting on Save
};

/**
 *
 */
UCLASS()
class AURA_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	/**
	 * Called once before an object gets serialized for saving
	 * We're going to use this to sort (Based on UCommonInputBaseControllerData::PreSave)
	 */
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

	/** Data Asset list of all abilities with icons, ... */
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow, TitleProperty="{AbilityName}"), Category="AbilityInformation")
	TArray<FAuraAbilityData> AbilityDataList;

	//TODO: Replace AbilityDataList with this and finish PreSave
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow, TitleProperty="{AbilityName}"), Category="AbilityInformation")
	TMap<TSubclassOf<UAuraGameplayAbility>, FAuraAbilityData> AbilityDataMap;

	static const UAbilityDataAsset* Get(const UObject* WorldContextObject);
	static const FAuraAbilityData* GetDataFromGameState(const UObject* WorldContextObject, const UClass* AbilityClass);

	static void UnlockAbilityByLevel(class UAbilitySystemComponent* ASC, const int32 CharacterLevel);

	static const FAuraAbilityData* GetAbilityDataFromID(UAbilitySystemComponent* ASC, const int32 InputID);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	/*UFUNCTION(CallInEditor, Category="Populate Data")
	void PopulateDataAsset();*/
#endif
};
