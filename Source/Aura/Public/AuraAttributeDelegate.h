// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraAttributeDelegate.generated.h"

struct FAuraAttributeData;
// Custom Delegate to handle Attribute
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeInfoSignature, const FGameplayTag&, Tag, const FAuraAttributeData&, Info);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeDelegate : public UObject
{
	GENERATED_BODY()
	
};
