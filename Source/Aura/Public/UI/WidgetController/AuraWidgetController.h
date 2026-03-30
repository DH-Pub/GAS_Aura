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
	 * @param Character
	 * @param WC TObjectPtr ref or else nullptr
	 * @return
	 */
	template <typename ControllerT = UAuraWidgetController>
	static ControllerT* CreateOrGetWidgetController(TObjectPtr<ControllerT>& WC, class AAuraCharacterBase* Character)
	{
		if (WC == nullptr || Character != WC->Character)
		{
			if (WC == nullptr) WC = NewObject<ControllerT>(Character, ControllerT::StaticClass());
			WC->SetCharacter(Character);
			WC->BindCallbacksDependencies();
		}
		return WC;
	}

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
	/**
	 * TODO: Instead of Character, Add GetAuraASC() or GetAuraAttributeSet() to ICombatInterface
	 * because owner of ASC and AS can be non-Character (PlayerState)
	 */
	/*UPROPERTY()
	TScriptInterface<class ICombatInterface> SourceInterface;*/
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> Character;
	void SetCharacter(AAuraCharacterBase* InCharacter);
};
