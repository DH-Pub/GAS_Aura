// Copyright Hung


#include "AbilitySystem/Data/AttributeDataAsset.h"

#include "AuraGameplayTags.h"

#if WITH_EDITOR
void UAttributeDataAsset::PopulateDataAsset()
{
	for (TTuple<FGameplayTag, FAuraAttributeData>& Info : AttributeDataList)
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
