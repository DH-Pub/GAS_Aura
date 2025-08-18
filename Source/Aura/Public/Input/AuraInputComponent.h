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
		UEnhancedInputLocalPlayerSubsystem* EnhancedInputWorldSubsystem, UserClassT* Object, TriggerFuncT TriggerFunc)
	{
		check(InputConfig);
		for (const auto& [InputTag, InputAction, OtherTrigger] : InputConfig->AbilityInputActions)
		{
			if (InputAction && InputTag.IsValid())
			{
				/* FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent,
				UserClass* Object, typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... > Func, VarTypes... Vars) */
				BindAction(InputAction, ETriggerEvent::Triggered, Object, TriggerFunc, ETriggerEvent::Triggered, InputTag);
				BindAction(InputAction, ETriggerEvent::Started, Object, TriggerFunc, ETriggerEvent::Started, InputTag);
				BindAction(InputAction, ETriggerEvent::Ongoing, Object, TriggerFunc, ETriggerEvent::Ongoing, InputTag);
				BindAction(InputAction, ETriggerEvent::Canceled, Object, TriggerFunc, ETriggerEvent::Canceled, InputTag);
				BindAction(InputAction, ETriggerEvent::Completed, Object, TriggerFunc, ETriggerEvent::Completed, InputTag);
			}
			for (const TPair<TEnumAsByte<EAuraTriggerType>, TObjectPtr<UInputAction>>& Pair : OtherTrigger)
			{
				if (Pair.Value && EnhancedInputWorldSubsystem)
				{
					for (const auto& Key : EnhancedInputWorldSubsystem->QueryKeysMappedToAction(InputAction))
					{
						InputMappingContext->MapKey(Pair.Value, Key);
					}
					BindAction(Pair.Value, ETriggerEvent::Triggered, Object, TriggerFunc, ETriggerEvent::Triggered, InputTag);
					BindAction(Pair.Value, ETriggerEvent::Started, Object, TriggerFunc, ETriggerEvent::Started, InputTag);
					BindAction(Pair.Value, ETriggerEvent::Ongoing, Object, TriggerFunc, ETriggerEvent::Ongoing, InputTag);
					BindAction(Pair.Value, ETriggerEvent::Canceled, Object, TriggerFunc, ETriggerEvent::Canceled, InputTag);
					BindAction(Pair.Value, ETriggerEvent::Completed, Object, TriggerFunc, ETriggerEvent::Completed, InputTag);
				}
			}
			UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(InputMappingContext);
		}
	}
};
