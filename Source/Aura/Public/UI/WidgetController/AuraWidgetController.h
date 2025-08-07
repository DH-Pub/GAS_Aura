// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

class AAuraPlayerController;
class UAuraAbilitySystemComponent;
class UAuraAttributeSet;

// Custom Delegate to handle Attribute
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerPointsChangedSignature, int32, NewValue, int32, PointsAllocating);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVitalAttributeChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityDataSignature, const FAuraAbilityDataAsset&, Info);

// Used to set Controller, State, ASC, AttributeSet
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams(){}
	// Use this when ASC and AS has not properly initialized
	FWidgetControllerParams(AAuraPlayerController* PC, AAuraPlayerState* PS, UAuraAbilitySystemComponent* ASC, UAuraAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}
	explicit FWidgetControllerParams(AAuraPlayerController* PC);

	// explicit  FWidgetControllerParams(AAuraPlayerController* PC);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AAuraPlayerController> PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AAuraPlayerState> PlayerState = nullptr;

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
	void SetWidgetControllerParams(const FWidgetControllerParams& WCParams);

	// Override, bind callbacks 
	virtual void BindCallbacksDependencies() {};
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues() {};

	UPROPERTY(BlueprintAssignable, Category = "GAS|AbilityData")
	FAbilityDataSignature AbilityDataDelegate;// Send Ability's Tags, Icons, Assets, ...

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
	TObjectPtr<AAuraPlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AAuraPlayerState> PlayerState;
	
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
