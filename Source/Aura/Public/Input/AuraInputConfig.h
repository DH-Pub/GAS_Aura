// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Misc/DataValidation.h"
#include "AuraInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* InputAction = nullptr;
	
	UPROPERTY(EditDefaultsOnly, meta=(GameplayTagFilter="Controls"))
	FGameplayTag InputTag = FGameplayTag();
};

class UInputAction;
/**
 * 
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(GameplayTagFilter="Controls", TitleProperty="{InputTag} - {InputAction}"))
	TArray<FAuraInputAction> AbilityInputActions;


#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override
	{
		EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
		unsigned int i = 0;
		for (const auto& [InputAction, InputTag] : AbilityInputActions)
		{
			if (InputAction == nullptr || !InputTag.IsValid())
			{
				Result = EDataValidationResult::Invalid;
				const FText ErrorMsg = FText::FromString("Tag and Input are required!!!");
				Context.AddError(ErrorMsg);
			}
		}
		return Result;
	}
#endif
};
