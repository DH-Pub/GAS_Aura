// Copyright Hung


#include "AbilitySystem/Data/AttributeDataAsset.h"

#include "AuraTag.h"

#if WITH_EDITOR
void UAttributeDataAsset::PopulateDataAsset()
{
	for (auto& [Tag, AttributeData] : AttributeDataList)
	{
		const FName TagName = Tag.GetTagName();
		FString TagComment;
		FName TagSource;
		bool bIsTagExplicit = false;
		bool bIsRestrictedTag = false;
		bool bAllowNonRestrictedChildren = false;
		UGameplayTagsManager::Get().GetTagEditorData(
			TagName, TagComment, TagSource, bIsTagExplicit, bIsRestrictedTag, bAllowNonRestrictedChildren);
		AttributeData.AttributeDescription = FText::FromString(TagComment);
	}
}
#endif
