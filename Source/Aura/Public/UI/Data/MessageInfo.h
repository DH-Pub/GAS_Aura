// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "Engine/DataAsset.h"
#include "MessageInfo.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthCrystal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_HealthPotion)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaCrystal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_ManaPotion)

USTRUCT(BlueprintType)
struct FAuraMessageInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UAuraUserWidget> MessageWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

/**
 * 
 */
UCLASS()
class AURA_API UMessageInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	FAuraMessageInfo FindMessageInfoForTag(const FGameplayTag& MessageTag) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(TitleProperty="Message", ForceInlineRow))
	TMap<FGameplayTag, FAuraMessageInfo> MessageInformation;
};
