// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Aura/Aura.h"
#include "Engine/DataAsset.h"
#include "Misc/DataValidation.h"
#include "AuraInputConfig.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<EAuraAbilityInputID, TObjectPtr<class UInputAction>> InputIDActions;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
	{
		EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
		for (const auto [InputID, InputAction] : InputIDActions)
		{
			if (InputID == EAuraAbilityInputID::None)
			{
				Result = EDataValidationResult::Invalid;
				const FText ErrorMsg = FText::FromString("InputID can't be None!!!");
				Context.AddError(ErrorMsg);
			}
			if (InputAction == nullptr)
			{
				Result = EDataValidationResult::Invalid;
				const FText ErrorMsg = FText::FromString("InputAction is required!!!");
				Context.AddError(ErrorMsg);
			}
		}
		return Result;
	}
#endif
};
