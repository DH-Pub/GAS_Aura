// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

/* TODO: Next proj: For some Widgets, just put logic inside them directly
 * use BlueprintImplementableEvent/BlueprintNativeEvent instead of DynamicDelegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVitalAttributeChanged, float, NewValue);

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	// bind callbacks, called when first created in CreateOrGetWidgetController
	virtual void BindCallbacksDependencies(){}
	/**
	 * Call this after Event WidgetControllerSet / Event Construct
	 * if Subclass has this function
	 */
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues(){} // If SetWidgetController is not called, call this

	/**
	 * Create WidgetController if none and BindCallbacksDependencies()
	 * @tparam ControllerT
	 * @param Outer
	 * @param InCharacter
	 * @param WC TObjectPtr ref or else nullptr
	 * @param WCClass
	 * @return
	 */
	template <typename ControllerT = UAuraWidgetController>
	static ControllerT* CreateOrGetWidgetController(UObject* Outer, class AAuraCharacterBase* InCharacter,
		TObjectPtr<ControllerT>& WC, const TSubclassOf<UAuraWidgetController> WCClass)
	{
		checkf(WCClass, TEXT("Widget Controller Class uninitialized, please fill out in BP_AuraHUD"));
		if (WC == nullptr || InCharacter != WC->Character)
		{
			if (WC == nullptr) WC = NewObject<ControllerT>(Outer, WCClass);
			WC->SetCharacter(InCharacter);
			WC->BindCallbacksDependencies();
		}
		return WC;
	}

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="PlayerController"))
	AController* GetPlayerController() const;
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="PlayerState"))
	class AAuraPlayerState* GetPlayerState() const; // Do not call this in AI's WidgetController
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="ASC"))
	UAuraAbilitySystemComponent* GetASC() const;
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="AttributeSet"))
	class UAuraAttributeSet* GetAttributeSet() const;

	UPROPERTY()
	TObjectPtr<class AAuraHUD> AuraHUD;
protected:
	// Bind AbilitySystemComponent's FOnGameplayAttributeValueChange to
	template<typename DelegateT = TBaseDynamicMulticastDelegate>
	void BindGameplayAttributeToBroadcast(const FGameplayAttribute& Attribute, const DelegateT& ChangedDelegate)
	{
		FOnGameplayAttributeValueChange& OnChanged = GetASC()->GetGameplayAttributeValueChangeDelegate(Attribute);
		if (!OnChanged.IsBoundToObject(this))
		{
			OnChanged.AddWeakLambda(this, [&ChangedDelegate](const FOnAttributeChangeData& Data)
			{
				ChangedDelegate.Broadcast(Data.NewValue);
			});
		}
	}
private:
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> Character;
	void SetCharacter(AAuraCharacterBase* InCharacter);
};
