// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

/* TODO: Next proj: For some Widgets, just bind MulticastDelegate from ASC/AS inside them directly
 * Call BlueprintImplementableEvent/BlueprintNativeEvent instead of DynamicDelegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVitalAttributeChanged, float, NewValue);

/**
 * For UI to Receive from FAbilityDataSignature AbilityDataDelegate
 * For Widget to receive info regardless of which ASC, WidgetController will handle binding to ASC
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReceiveAbilityDataSignature);

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	void SetAbilitySystemComponent(UAuraAbilitySystemComponent* InASC)
	{
		if (AuraASC == InASC) return;
		if (AuraASC)
		{
			UnbindOldAbilitySystemComponent();
		}
		AuraASC = InASC;
		if (AuraASC)
		{
			BindCallbacksDependencies();
		}
	};

	// IMPORTANT: Override this in child class
	virtual void UnbindOldAbilitySystemComponent() {}
	// IMPORTANT: Override this in child class
	virtual void BindCallbacksDependencies() {}

	/**
	 * This should be called after Event BP_SetWidgetController / Event Construct
	 * Usually, broadcast everything that was bound in BindCallbacksDependencies()
	 */
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues(){}

	/**
	 * Create WidgetController if none for OutWC, and set ASC
	 * @tparam ControllerT UAuraWidgetController
	 * @param OutWC TObjectPtr ref or else nullptr
	 * @param InASC Ability System Component to bind to
	 * @return
	 */
	template <typename ControllerT = UAuraWidgetController>
	static void CreateOrGetWidgetController(TObjectPtr<ControllerT>& OutWC, UAuraAbilitySystemComponent* InASC)
	{
		if (OutWC == nullptr)
		{
			OutWC = NewObject<ControllerT>(InASC, ControllerT::StaticClass());
		}
		OutWC->SetAbilitySystemComponent(InASC);
	}

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="ASC"))
	UAuraAbilitySystemComponent* GetASC() const {return AuraASC;}

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="AttributeSet"))
	const class UAuraAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="PlayerState"))
	class AAuraPlayerState* GetPlayerState() const; // nullptr for AI's WidgetController

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="AttributeSet"))
	class AAuraCharacterBase* GetAuraCharacter() const;
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="Controller"))
	AController* GetCharacterController() const;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraASC;
protected:
	// Bind AbilitySystemComponent's FOnGameplayAttributeValueChange to
	template<typename DelegateT = TBaseDynamicMulticastDelegate>
	void BindGameplayAttributeToBroadcast(const FGameplayAttribute& Attribute, const DelegateT& ChangedDelegate)
	{
		GetASC()->GetGameplayAttributeValueChangeDelegate(Attribute).AddWeakLambda(this,
		[&ChangedDelegate](const FOnAttributeChangeData& Data)
		{
			ChangedDelegate.Broadcast(Data.NewValue);
		});
	}
};
