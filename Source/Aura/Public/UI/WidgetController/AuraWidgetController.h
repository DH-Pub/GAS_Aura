// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

class UAuraAbilitySystemComponent;
class UAuraAttributeSet;

// Custom Delegate to handle Attribute
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

// Used to set Controller, State, ASC, AttributeSet
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams(){}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAuraAbilitySystemComponent* ASC, UAuraAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);
	
	// Override, bind callbacks 
	virtual void BindCallbacksDependencies() {};
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues() {};


	// Create WidgetController if none and BindCallbacksDependencies()
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

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
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
