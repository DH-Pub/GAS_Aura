// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 *  MUST change Project Settings -> Engine/Input -> Default Input Component Classes
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	template<class UserClassT, typename PressedFuncT, typename ReleasedFuncT, typename HeldFuncT>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClassT* Object, PressedFuncT PressedFunc, ReleasedFuncT ReleasedFunc, HeldFuncT HeldFunc)
	{
		check(InputConfig);
		for (const TTuple<FGameplayTag, TObjectPtr<UInputAction>>& Action : InputConfig->AbilityInputActions)
		{
			if (Action.Value && Action.Key.IsValid())
			{
				if (PressedFunc) BindAction(Action.Value, ETriggerEvent::Started, Object, PressedFunc, Action.Key);
				if (ReleasedFunc) BindAction(Action.Value, ETriggerEvent::Completed, Object, ReleasedFunc, Action.Key);
				if (HeldFunc) BindAction(Action.Value, ETriggerEvent::Triggered, Object, HeldFunc, Action.Key);
			}
		}
	}
};

