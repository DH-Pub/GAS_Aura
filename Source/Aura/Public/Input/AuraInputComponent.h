// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
// #include "EnhancedInputSubsystemInterface.h"
// #include "EnhancedInputSubsystems.h"
// #include "InputMappingContext.h"
#include "AuraInputComponent.generated.h"

enum class ETriggerEvent : uint8;
/**
 *  MUST change Project Settings -> Engine/Input -> Default Input Component Classes
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	// Bind UAuraInputConfig's InputAction:
	template<class UserClassT, typename PressedFuncT, typename ReleasedFuncT>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UInputMappingContext* InputMappingContext,
		UserClassT* Object, PressedFuncT PressedFunc, ReleasedFuncT ReleasedFunc)
	{
		check(InputConfig);
		for (const auto [InputID, InputAction] : InputConfig->InputIDActions)
		{
			const int8 Idx = static_cast<int8>(InputID);
			if (InputAction && Idx > 0)
			{	/*template FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ...)*/
				if (PressedFunc) BindAction(InputAction, ETriggerEvent::Started, Object, PressedFunc, Idx);
				if (ReleasedFunc) BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Idx);
			}
			/*for (const auto& Key : EnhancedInputLocalPlayerSubsystem->QueryKeysMappedToAction(InputAction))
			{InputMappingContext->MapKey(InputAction, Key);}*/
			UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(InputMappingContext);
		}
	}
};
