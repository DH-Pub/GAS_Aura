// Copyright Hung


#include "AbilitySystem/Data/AbilityDataAsset.h"

#include "AuraGameplayTags.h"
#include "Aura/AuraLogChannels.h"

FAuraAbilityDataAsset UAbilityDataAsset::FindAbilityDataByTag(const FGameplayTagContainer& AbilityTags, bool bLogNotFound)
{
	for (FAuraAbilityDataAsset& Data : AbilityData)
	{
		for (FGameplayTag Tag : AbilityTags)
		{
			if (Data.AbilityTag == Tag && Tag.MatchesTag(AuraGameplayTags::Abilities)) return Data;
		}
	}

	if (bLogNotFound) UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag on AbilityData [%s]"), *GetNameSafe(this));
	return FAuraAbilityDataAsset();
}
