// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Game/AuraGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"

const UAbilityDataAsset* UAbilityDataAsset::GetFromGameState(const UObject* WorldContextObject)
{
	if (AAuraGameState* GameState = Cast<AAuraGameState>(UGameplayStatics::GetGameState(WorldContextObject)))
	{
		return GameState->AbilityDataAsset;
	}
	return nullptr;
}

const FAuraAbilityData* UAbilityDataAsset::GetAbilityFromGameState(const UObject* WorldContextObject, const FGameplayTag& Tag)
{
	if (const UAbilityDataAsset* DA = GetFromGameState(WorldContextObject))
	{
		for (const FAuraAbilityData& Data : DA->AbilityDataList) if (Tag.MatchesTagExact(Data.AbilityTag)) return &Data;
	}
	return nullptr;
}
const FAuraAbilityData* UAbilityDataAsset::GetAbilityFromGameState(const UObject* WorldContextObject,
	const FGameplayTagContainer& Tags)
{
	if (const UAbilityDataAsset* DA = GetFromGameState(WorldContextObject))
	{
		for (const FAuraAbilityData& Data : DA->AbilityDataList) {if (Tags.HasTagExact(Data.AbilityTag)) return &Data;}
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
			Context.AddError(FText::FromString("Tags are required!!!"));
		}
		if (AbilityData.Icon == nullptr || AbilityData.BackgroundMaterial == nullptr
			|| AbilityData.LevelRequirement <= 0 || AbilityData.AbilityClass == nullptr)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::FromString("Tags are required!!!"));
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
