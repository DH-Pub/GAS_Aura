// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

class AAuraHUD;
class AAuraPlayerController;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVitalAttributeChanged, float, NewValue);

// Used to set Controller, State, ASC, AttributeSet
USTRUCT()
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams(){}
	// Use this when ASC and AS has not properly initialized
	FWidgetControllerParams(AAuraPlayerController* PC, AAuraPlayerState* PS, UAuraAbilitySystemComponent* ASC, UAuraAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}
	explicit FWidgetControllerParams(AAuraPlayerController* PC); // When ASC and AS are initialized

	UPROPERTY()
	TObjectPtr<AAuraPlayerController> PlayerController = nullptr;
	UPROPERTY()
	TObjectPtr<AAuraPlayerState> PlayerState = nullptr;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY()
	TObjectPtr<UAuraAttributeSet> AttributeSet = nullptr;
};

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
	{
		PlayerController = WCParams.PlayerController;
		PlayerState = WCParams.PlayerState;
		AbilitySystemComponent = WCParams.AbilitySystemComponent;
		AttributeSet = WCParams.AttributeSet;
	}

	// bind callbacks, called when first created in CreateOrGetWidgetController
	virtual void BindCallbacksDependencies() {};
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues() {};

	// Create WidgetController if none and BindCallbacksDependencies()
	// Example: UserWidget.h WidgetT* CreateWidget
	template <typename ControllerT = UAuraWidgetController>
	static ControllerT* CreateOrGetWidgetController(UObject* Outer, TObjectPtr<ControllerT>& WC,
		const TSubclassOf<UAuraWidgetController> WCClass, const FWidgetControllerParams& WCParams)
	{
		checkf(WCClass, TEXT("Widget Controller Class uninitialized, please fill out in BP_AuraHUD"));
		if (WC == nullptr)
		{
			WC = NewObject<ControllerT>(Outer, WCClass);
			WC->SetWidgetControllerParams(WCParams);
			WC->BindCallbacksDependencies();
		}
		return WC;
	}

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraPlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraPlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAuraAttributeSet> AttributeSet;

	// Bind AbilitySystemComponent's FOnGameplayAttributeValueChange to 
	template<typename DelegateT = TBaseDynamicMulticastDelegate>
	void BindGameplayAttributeToBroadcast(const FGameplayAttribute& Attribute, const DelegateT& AttributeChanged)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute)
		.AddLambda([&AttributeChanged](const FOnAttributeChangeData& Data)
		{AttributeChanged.Broadcast(Data.NewValue);});
	}
};
