// Copyright Hung


#include "AbilitySystem/Data/AttributeInfo.h"

#include "AuraGameplayTags.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	return AttributeInformation.FindRef(AttributeTag);
	
	/*for (TTuple<FGameplayTag, FAuraAttributeInfo> Info : AttributeInformation)
	{
		if (Info.Key.MatchesTagExact(AttributeTag)) // Info.Key == AttributeTag
		{
			return Info.Value;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(), *GetNameSafe(this));
	}
	return FAuraAttributeInfo();*/
}


#if WITH_EDITOR
void UAttributeInfo::PopulateDataAsset()
{
	for (TTuple<FGameplayTag, FAuraAttributeInfo>& Info : AttributeInformation)
	{
		const FName TagName = Info.Key.GetTagName();
		FString TagComment;
		FName TagSource;
		bool bIsTagExplicit = false;
		bool bIsRestrictedTag = false;
		bool bAllowNonRestrictedChildren = false;
		UGameplayTagsManager::Get().GetTagEditorData(
			TagName, TagComment, TagSource, bIsTagExplicit, bIsRestrictedTag, bAllowNonRestrictedChildren);
		Info.Value.AttributeDescription = FText::FromString(TagComment);
	}
}
#endif
