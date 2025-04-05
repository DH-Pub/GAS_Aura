// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAuraAttributeInfo
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
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="{AttributeName} - {GameplayAttribute}", ForceInlineRow))
	TMap<FGameplayTag, FAuraAttributeInfo> AttributeInformation;


	
#if WITH_EDITOR // Create a button in editor
	UFUNCTION(Category="Populate Attribute Info", CallInEditor)
	void PopulateDataAsset();
#endif
};
