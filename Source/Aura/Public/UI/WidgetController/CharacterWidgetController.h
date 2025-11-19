// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "CharacterWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUpSignature, int32, Level);

/**
 *
 */
UCLASS()
class UCharacterWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category="GAS")
	FOnLevelUpSignature OnLevelUpDelegate;
};
