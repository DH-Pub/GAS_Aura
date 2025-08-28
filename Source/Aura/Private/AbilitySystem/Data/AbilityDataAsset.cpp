// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Misc/DataValidation.h"

FAuraAbilityData* UAbilityDataAsset::FindAbilityDataByTags(const FGameplayTagContainer& AbilityTags)
{
	for (FAuraAbilityData& Data : AbilityDataList)
	{
		if (AbilityTags.HasTagExact(Data.AbilityTag)) return &Data;
	}
	// UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag on List [%s]"), *GetNameSafe(this));
	return nullptr;
}
FAuraAbilityData* UAbilityDataAsset::FindAbilityDataByTags(const FGameplayTag& AbilityTags)
{
	for (FAuraAbilityData& Data : AbilityDataList)
	{
		if (AbilityTags.MatchesTagExact(Data.AbilityTag)) return &Data;
	}
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult UAbilityDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	// return Super::IsDataValid(Context);
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	for (auto& AbilityData : AbilityDataList)
	{
		if (!AbilityData.AbilityTag.IsValid())
		{
			Result = EDataValidationResult::Invalid;
			const FText ErrorMsg = FText::FromString("Tags are required!!!");
			Context.AddError(ErrorMsg);
		}
		if (AbilityData.Icon == nullptr || AbilityData.BackgroundMaterial == nullptr
			|| AbilityData.LevelRequirement <= 0 || AbilityData.AbilityClass == nullptr)
		{
			Result = EDataValidationResult::Invalid;
			const FText ErrorMsg = FText::FromString("Tags are required!!!");
			Context.AddError(ErrorMsg);
		}
	}
	return Result;
}
#endif
