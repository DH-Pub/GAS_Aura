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
	// Bind UAuraInputConfig's InputAction with Pressed, Released, Held Func
	template<class UserClassT, typename PressedFuncT, typename ReleasedFuncT, typename HeldFuncT>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClassT* Object,
		PressedFuncT PressedFunc, ReleasedFuncT ReleasedFunc, HeldFuncT HeldFunc)
	{
		check(InputConfig);
		for (const auto& [InputAction, InputTag] : InputConfig->AbilityInputActions)
		{
			if (InputAction && InputTag.IsValid())
			{
				/* FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent,
				UserClass* Object, typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... > Func, VarTypes... Vars) */
				if (PressedFunc) BindAction(InputAction, ETriggerEvent::Started, Object, PressedFunc, InputTag);
				if (ReleasedFunc) BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag);
				if (HeldFunc) BindAction(InputAction, ETriggerEvent::Triggered, Object, HeldFunc, InputTag);
			}
		}
	}
};

