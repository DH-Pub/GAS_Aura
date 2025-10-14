// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
// #include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
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
	template<class UserClassT, typename TriggerFuncT>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UInputMappingContext* InputMappingContext,
		UserClassT* Object, TriggerFuncT TriggerFunc)
	{
		check(InputConfig);
		for (const auto& [InputTag, InputAction] : InputConfig->AbilityInputActions)
		{
			if (InputAction && InputTag.IsValid())
			{
				/* FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent,
				UserClass* Object, typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... > Func, VarTypes... Vars) */
				BindAction(InputAction, ETriggerEvent::Triggered, Object, TriggerFunc, ETriggerEvent::Triggered, &InputTag, InputAction.Get());
				BindAction(InputAction, ETriggerEvent::Started, Object, TriggerFunc, ETriggerEvent::Started, &InputTag, InputAction.Get());
				BindAction(InputAction, ETriggerEvent::Ongoing, Object, TriggerFunc, ETriggerEvent::Ongoing, &InputTag, InputAction.Get());
				BindAction(InputAction, ETriggerEvent::Canceled, Object, TriggerFunc, ETriggerEvent::Canceled, &InputTag, InputAction.Get());
				BindAction(InputAction, ETriggerEvent::Completed, Object, TriggerFunc, ETriggerEvent::Completed, &InputTag, InputAction.Get());
			}
			/*for (const auto& Key : EnhancedInputLocalPlayerSubsystem->QueryKeysMappedToAction(InputAction))
			{
				InputMappingContext->MapKey(Pair.Value, Key);
			}*/
			UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(InputMappingContext);
		}
	}
};
