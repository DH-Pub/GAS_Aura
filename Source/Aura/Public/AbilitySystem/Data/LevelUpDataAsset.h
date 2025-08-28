// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FAuraLevelUpData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0, ClampMin=0, UIMax=100))
	int32 AttributePointsGain = 1;

	UPROPERTY(EditDefaultsOnly, meta=(UIMin=0, ClampMin=0, UIMax=100))
	int32 SpellPointsGain = 1;
};
/**
 * 
 */
UCLASS()
class AURA_API ULevelUpDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty="{LevelUpRequirement} Attr:{AttributePointsGain} Spell:{SpellPointsGain}"))
	TArray<FAuraLevelUpData> LevelUpDataList;

	int32 FindLevelForXP(int32 XP) const;
	int32 GetMaxLevel() const {return LevelUpDataList.Num() - 1;}

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
