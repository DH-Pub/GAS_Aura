// Copyright Hung


#include "AbilitySystem/Data/LevelUpDataAsset.h"

#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "XPError"

int32 ULevelUpDataAsset::FindLevelForXP(const int32 XP) const
{	// Binary Search Algorithm
	int32 Left = 1; // Min Level
	int32 Right = LevelUpDataList.Num() - 1; // Max Level
	while (Left < Right) // if sort by value increase
	{
		const int32 Mid = Left + (Right - Left) / 2;
		if (XP < LevelUpDataList[Mid].LevelUpRequirement) Right = Mid; // upper_bound, ignore right
		else Left = Mid + 1;
	}
	return Left; // Left == Right
}


#if WITH_EDITOR
EDataValidationResult ULevelUpDataAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	int32 LevelUpRequirement = -1;
	for (int32 i = 0; i < LevelUpDataList.Num(); i++)
	{
		if (LevelUpDataList[i].LevelUpRequirement > LevelUpRequirement)
		{
			LevelUpRequirement = LevelUpDataList[i].LevelUpRequirement;
		}
		else
		{
			Result = EDataValidationResult::Invalid;
			FFormatNamedArguments Args;
			Args.Add("Level", i);
			Args.Add("Requirement", LevelUpRequirement);
			const FText FormattedText = FText::Format(NSLOCTEXT("XPError", "XPFormat",
				"Next Level ({Level}) Up Requirement needs to be bigger than {Requirement}!!!"), Args);
			const FText ErrorMsg = FText::Format(LOCTEXT("Err",
				"Next Level ({0}) Up Requirement needs to be bigger than last!!!"), i);
			Context.AddError(FormattedText);
		}
	}
	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
