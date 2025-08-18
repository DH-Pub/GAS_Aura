// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Abilities/AuraInputAbility.h"
#include "Misc/DataValidation.h"

void FAuraAbilityData::SetInputAndStatusTag(const FGameplayAbilitySpec& AbilitySpec)
{
	InputTag = Cast<UAuraInputAbility>(AbilitySpec.Ability)->StartupInputTag;
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(AuraGameplayTags::Ability_Status)) {StatusTag = Tag; return;}
	}
}

FAuraAbilityData* UAbilityDataAsset::FindAbilityDataByTag(const FGameplayTagContainer& AbilityTags)
{
	for (FAuraAbilityData& Data : AbilityDataList)
	{
		if (AbilityTags.HasTagExact(Data.AbilityTag)) return &Data;
	}
	// UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag on List [%s]"), *GetNameSafe(this));
	return nullptr;
}

#if WITH_EDITOR
EDataValidationResult UAbilityDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	// return Super::IsDataValid(Context);
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	for (auto& AbilityData : AbilityDataList)
	{
		if (!AbilityData.AbilityTag.IsValid() || !AbilityData.CooldownTag.IsValid())
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
