// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraWidgetController.generated.h"

/* TODO: Next proj: For some Widgets, just bind MulticastDelegate from ASC/AS inside them directly
 * Call BlueprintImplementableEvent/BlueprintNativeEvent instead of DynamicDelegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVitalAttributeChanged, float, NewValue);

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * - Bind callbacks, called when first created in CreateOrGetWidgetController
	 * - IMPORTANT: Unbind previous callback before calling Super:: to set new AuraASC
	 */
	virtual void BindCallbacksDependencies(UAuraAbilitySystemComponent* InASC) {AuraASC = InASC;}

	/**
	 * This should be called after Event WidgetControllerSet / Event Construct
	 * Usually, broadcast everything that was bound in BindCallbacksDependencies()
	 */
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues(){}

	/**
	 * Create WidgetController if none for OutWC, and call BindCallbacksDependencies()
	 * @tparam ControllerT
	 * @param OutWC TObjectPtr ref or else nullptr
	 * @param InASC Ability System Component to bind to
	 * @return
	 */
	template <typename ControllerT = UAuraWidgetController>
	static ControllerT* CreateOrGetWidgetController(TObjectPtr<ControllerT>& OutWC, UAuraAbilitySystemComponent* InASC)
	{
		if (OutWC == nullptr)
		{
			OutWC = NewObject<ControllerT>(InASC, ControllerT::StaticClass());
		}
		OutWC->BindCallbacksDependencies(InASC);
		return OutWC;
	}

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="ASC"))
	UAuraAbilitySystemComponent* GetASC() const {return AuraASC;}

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="AttributeSet"))
	const class UAuraAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="PlayerState"))
	class AAuraPlayerState* GetPlayerState() const; // Do not call this in AI's WidgetController (nullptr)
	UFUNCTION(BlueprintPure, meta=(CompactNodeTitle="AttributeSet"))
	class AAuraCharacterBase* GetAuraCharacter() const;

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
