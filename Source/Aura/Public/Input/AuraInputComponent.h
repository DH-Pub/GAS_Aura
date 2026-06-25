// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
#include "Aura/Aura.h"
#include "Misc/DataValidation.h"
// #include "EnhancedInputSubsystemInterface.h"
// #include "EnhancedInputSubsystems.h"
// #include "InputMappingContext.h"
#include "AuraInputComponent.generated.h"

enum class ETriggerEvent : uint8;


/**
 *
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<TEnumAsByte<EAuraAbilityInputID::Type>, TObjectPtr<class UInputAction>> InputIDActions;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override
	{
		EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
		for (const auto [InputID, InputAction] : InputIDActions)
		{
			if (InputID < 1)
			{
				Result = EDataValidationResult::Invalid;
				const FText ErrorMsg = FText::FromString("InputID can't be Passives or None!!!");
				Context.AddError(ErrorMsg);
			}
			if (InputAction == nullptr)
			{
				Result = EDataValidationResult::Invalid;
				const FText ErrorMsg = FText::FromString("InputAction is required!!!");
				Context.AddError(ErrorMsg);
			}
		}
		return Result;
	}
#endif
};


/**
 * Store Common input
 */
UCLASS()
class AURA_API UAuraInputDataAsset : public UDataAsset /*UPrimaryDataAsset*/
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const class UAuraInputConfig> InputConfig;
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const UInputAction> MouseInputAction;

	/*virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("AuraInputDataAsset", GetFName());
	}*/
};


/**
 *  IMPORTANT: MUST change Project Settings -> Engine/Input -> Default Input Component Classes
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:
	// Bind UAuraInputConfig's InputAction:
	template<class UserClassT, typename PressedFuncT, typename ReleasedFuncT>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, const UInputMappingContext* InputMappingContext,
		UserClassT* Object, PressedFuncT PressedFunc, ReleasedFuncT ReleasedFunc)
	{	check(InputConfig);
		for (const auto [InputID, InputAction] : InputConfig->InputIDActions)
		{	/*template FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ...)*/
			ensure(InputAction);
			if (PressedFunc) BindAction(InputAction, ETriggerEvent::Started, Object, PressedFunc, InputID.GetIntValue());
			if (ReleasedFunc) BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputID.GetIntValue());

			/*for (const auto& Key : EnhancedInputLocalPlayerSubsystem->QueryKeysMappedToAction(InputAction))
			{InputMappingContext->MapKey(InputAction, Key);}*/
		}
		// UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(InputMappingContext);
	}
};
