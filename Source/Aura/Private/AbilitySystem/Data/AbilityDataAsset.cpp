// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraTag.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Game/AuraGameState.h"
#include "Misc/DataValidation.h"

const UAbilityDataAsset* UAbilityDataAsset::Get(const UObject* WorldContextObject)
{
	AAuraGameState* GameState = AAuraGameState::Get(WorldContextObject);
	return GameState ? GameState->AbilityDataAsset : nullptr;
}

const FAuraAbilityData* UAbilityDataAsset::GetDataFromGameState(const UObject* WorldContextObject,
	const UClass* AbilityClass)
{
	if (const UAbilityDataAsset* DA = Get(WorldContextObject))
	{
		for (const FAuraAbilityData& Data : DA->AbilityDataList)
		{
			if (Data.AbilityClass == AbilityClass) return &Data;
		}
	}
	return nullptr;
}

void UAbilityDataAsset::UnlockAbilityByLevel(UAbilitySystemComponent* ASC, const int32 CharacterLevel)
{
	for (const FAuraAbilityData& Data : Get(ASC)->AbilityDataList)
	{	/* not enough lv or already has ability */
		if (CharacterLevel < Data.LevelRequirement || ASC->FindAbilitySpecFromClass(Data.AbilityClass)) continue;
		FGameplayAbilitySpec AbilitySpec(Data.AbilityClass, 1);
		if (Data.bAutoUnlock)
		{

		}
		else
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTagFast(AuraTag::Ability_Status_Eligible);
		}
		ASC->GiveAbility(AbilitySpec);
	}
}

const FAuraAbilityData* UAbilityDataAsset::GetAbilityDataFromID(UAbilitySystemComponent* ASC, const int32 InputID)
{
	if (const FGameplayAbilitySpec* Spec = ASC ? ASC->FindAbilitySpecFromInputID(InputID) : nullptr)
	{
		return GetDataFromGameState(ASC, Spec->Ability.GetClass());
	}
	return nullptr;
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
