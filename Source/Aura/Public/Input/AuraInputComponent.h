// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
// #include "EnhancedInputSubsystemInterface.h"
// #include "EnhancedInputSubsystems.h"
#include "Player/AuraPlayerController.h"
#include "InputMappingContext.h"
#include "AuraInputComponent.generated.h"

class AAuraPlayerController;
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
		for (const auto [InputTag, InputAction] : InputConfig->AbilityInputActions)
		{
			if (InputAction && InputTag.IsValid())
			{	/* FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent,
				UserClass* Object, typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... > Func, VarTypes... Vars) */
				if (PressedFunc)
				{
					BindAction(InputAction, ETriggerEvent::Started, Object, PressedFunc, InputTag);
					BindAction(InputAction, ETriggerEvent::Triggered, Object, PressedFunc, InputTag);
				}
				if (ReleasedFunc) BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag);
			}
			/*for (const auto& Key : EnhancedInputLocalPlayerSubsystem->QueryKeysMappedToAction(InputAction))
			{InputMappingContext->MapKey(Pair.Value, Key);}*/
			UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(InputMappingContext);
		}
	}
};
