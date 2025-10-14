// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Misc/DataValidation.h"
#include "AuraInputConfig.generated.h"

UENUM(BlueprintType)
enum EAuraTriggerType : uint8
{
	Hold, // Default, 
	RepeatedTap,
	Pulse,
	ChorededAction,
	Combo
};

class UInputAction;

USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(GameplayTagFilter="Input"))
	FGameplayTag InputTag = FGameplayTag();
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;
};
/**
 * 
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(GameplayTagFilter="Input", TitleProperty="{InputTag} - {InputAction}"))
	TArray<FAuraInputAction> AbilityInputActions;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
	{
		EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
		for (const auto& [InputTag, InputAction] : AbilityInputActions)
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
