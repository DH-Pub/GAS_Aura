// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText(); // for widget

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute GameplayAttribute;
};

/**
 * 
 */
UCLASS()
class AURA_API UAttributeDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	FAuraAttributeData FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const
	{return AttributeDataList.FindRef(AttributeTag);}

	/**
	 * Future note: make this a TArray
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="{AttributeName} - {GameplayAttribute}", ForceInlineRow, GameplayTagFilter="Attributes"))
	TMap<FGameplayTag, FAuraAttributeData> AttributeDataList;
	
	/**
	 * Editor
	 */
#if WITH_EDITOR // Create a button in editor
	UFUNCTION(Category="Populate Attribute Info", CallInEditor)
	void PopulateDataAsset();
#endif
};
