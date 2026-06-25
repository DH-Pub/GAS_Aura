// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "AuraButtonBase.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UAuraButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
protected:
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
};
