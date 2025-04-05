// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams(){}
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
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

	template <typename ControllerT = UAuraWidgetController>
	static ControllerT* CreateOrGetWidgetController(UObject* Outer, TObjectPtr<ControllerT>& WC, TSubclassOf<UAuraWidgetController> WCClass, const FWidgetControllerParams& WCParams)
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
protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

	template<typename DelegateT = TBaseDynamicMulticastDelegate>
	void BindGameplayAttributeToBroadcast(const struct FGameplayAttribute& Attribute, const DelegateT& AttributeChanged)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
			[&AttributeChanged](const FOnAttributeChangeData& Data)
			{
				AttributeChanged.Broadcast(Data.NewValue);
			}
		);
	}
};
