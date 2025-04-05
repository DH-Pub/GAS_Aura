// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Misc/DataValidation.h"
#include "AuraInputConfig.generated.h"

// USTRUCT(BlueprintType)
// struct FAuraInputAction
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditDefaultsOnly)
// 	const class UInputAction* InputAction = nullptr;
//
// 	UPROPERTY(EditDefaultsOnly)
// 	const FGameplayTag InputTag = FGameplayTag();
// };

class UInputAction;
/**
 * 
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ForceInlineRow))
	TMap<FGameplayTag, TObjectPtr<UInputAction>> AbilityInputActions;

	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override
	{
		EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
		unsigned int i = 0;
		for (const TTuple<FGameplayTag, TObjectPtr<UInputAction>>& Input : AbilityInputActions)
		{
			if (!Input.Key.IsValid() || Input.Value == nullptr)
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
