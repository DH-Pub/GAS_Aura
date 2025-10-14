// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "TextDataAsset.generated.h"

class UAuraUserWidget;

#pragma region Message
USTRUCT()
struct FMessageRow : public FTableRowBase // FAuraMessageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(GameplayTagFilter="Message"))
	FGameplayTag MessageTag = FGameplayTag(); // Set RowName the same as this

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine))
	FText Message = FText();
};
USTRUCT(BlueprintType)
struct FAuraMessageInfo // Use this or FMessageRow (FTableRowBase)
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Image = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine))
	FText Message = FText();
};
/*
 * 
 */
UCLASS()
class AURA_API UMessageInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	const FAuraMessageInfo* FindMessageInfoForTag(const FGameplayTag& MessageTag) const
	{return MessageInformation.Find(MessageTag);}
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Message", ForceInlineRow, GameplayTagFilter="Message"))
	TMap<FGameplayTag, FAuraMessageInfo> MessageInformation;
};
#pragma endregion
