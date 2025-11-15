// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Game/AuraGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/DataValidation.h"

const FGameplayTag& FAuraAbilityData::GetAuraAbilityTag() const
{
	return AbilityClass ? AbilityClass->GetDefaultObject<UAuraGameplayAbility>()->AuraAbilityTag : FGameplayTag::EmptyTag;
}

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
		for (const FAuraAbilityData& Data : DA->AbilityDataList) if (Tag.MatchesTagExact(Data.GetAuraAbilityTag())) return &Data;
	}
	return nullptr;
}
const FAuraAbilityData* UAbilityDataAsset::GetAbilityFromGameState(const UObject* WorldContextObject,
	const FGameplayTagContainer& Tags)
{
	if (const UAbilityDataAsset* DA = GetFromGameState(WorldContextObject))
	{
		for (const FAuraAbilityData& Data : DA->AbilityDataList) {if (Tags.HasTagExact(Data.GetAuraAbilityTag())) return &Data;}
	}
	return nullptr;
}
const FAuraAbilityData* UAbilityDataAsset::GetAbilityFromGameState(const UObject* WorldContextObject,
	const TSubclassOf<UAuraGameplayAbility> AbilityClass)
{
	if (const UAbilityDataAsset* DA = GetFromGameState(WorldContextObject))
	{
		for (const FAuraAbilityData& Data : DA->AbilityDataList) {if (AbilityClass == Data.AbilityClass) return &Data;}
	}
	return nullptr;
}

const FGameplayTag& UAbilityDataAsset::GetAbilityTagFromClass(const UObject* WorldContextObject,
	const TSubclassOf<UAuraGameplayAbility> AbilityClass)
{
	if (const FAuraAbilityData* Data = GetAbilityFromGameState(WorldContextObject, AbilityClass))
	{
		return Data->GetAuraAbilityTag();
	}
	return FGameplayTag::EmptyTag;
}

#if WITH_EDITOR
EDataValidationResult UAbilityDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	for (auto& AbilityData : AbilityDataList)
	{
		if (AbilityData.AbilityClass == nullptr)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::FromString("Ability Class Required!!!"));
		}
	}
	return Result; // return Super::IsDataValid(Context);
}
#endif
