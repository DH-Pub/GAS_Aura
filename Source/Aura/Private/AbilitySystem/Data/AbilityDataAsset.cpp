// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Misc/DataValidation.h"

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

void UAbilityDataAsset::PopulateDataAsset()
{
	for (FAuraAbilityData& Data : AbilityDataList)
	{
		if (Data.AbilityClass == nullptr || Data.AbilityTag.IsValid()) continue;
		for (const FGameplayTag& Tag : Data.AbilityClass.GetDefaultObject()->GetAssetTags())
		{
			if (Tag.MatchesTag(AuraGameplayTags::Ability)) Data.AbilityTag = Tag;
		}
	}
}
#endif
