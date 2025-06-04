// Copyright Hung


#include "AbilitySystem/Data/AttributeInfo.h"

#include "AuraGameplayTags.h"

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
