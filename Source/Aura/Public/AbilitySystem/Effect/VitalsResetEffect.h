// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "VitalsResetEffect.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UVitalsResetEffect : public UGameplayEffect //NOTE: Cannot Start class name with "Reset"
{
	GENERATED_BODY()
public:
	UVitalsResetEffect();
};
