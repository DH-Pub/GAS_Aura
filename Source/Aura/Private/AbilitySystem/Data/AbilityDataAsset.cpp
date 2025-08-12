// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "Aura/AuraLogChannels.h"

FAuraAbilityData* UAbilityDataAsset::FindAbilityDataByTag(const FGameplayTagContainer& AbilityTags)
{
	for (FAuraAbilityData& Data : AbilityDataList)
	{
		if (AbilityTags.HasTagExact(Data.AbilityTag)) return &Data;
	}
	// UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag on List [%s]"), *GetNameSafe(this));
	return nullptr;
}
