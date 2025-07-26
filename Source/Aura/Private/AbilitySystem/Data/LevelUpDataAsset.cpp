// Copyright Hung


#include "AbilitySystem/Data/LevelUpDataAsset.h"

int32 ULevelUpDataAsset::FindLevelForXP(const int32 XP) const
{
	// Binary Search Algorithm
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
