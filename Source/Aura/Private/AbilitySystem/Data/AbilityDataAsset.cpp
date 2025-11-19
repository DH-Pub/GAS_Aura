// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
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

void UAbilityDataAsset::UnlockAbilityByLevel(const UObject* WorldContextObject, UAuraAbilitySystemComponent* ASC,
	const int32 CharacterLevel)
{
	for (const FAuraAbilityData& Data : GetFromGameState(WorldContextObject)->AbilityDataList)
	{	/* not enough lv or already has ability */
		if (CharacterLevel < Data.LevelRequirement || ASC->GetSpecFromAbilityTag(Data.GetAuraAbilityTag())) continue;
		FGameplayAbilitySpec AbilitySpec(Data.AbilityClass, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTagFast(AuraGameplayTags::Ability_Status_Eligible);
		ASC->GiveAbility(AbilitySpec);
	}
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
