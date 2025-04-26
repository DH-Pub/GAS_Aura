// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

class UAuraAttributeSet;
/**
 * DEPRECATED
 */
UCLASS()
class AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	UDamageTextComponent();

	// BlueprintImplementableEvent can't be NetMulticast so we have to call BP function through a NetMulticast function
	UFUNCTION(NetMulticast, Unreliable)
	void SetDamageText(float Damage);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BP_SetDamageText(float Damage);
};
