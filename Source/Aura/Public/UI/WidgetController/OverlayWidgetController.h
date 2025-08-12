// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UI/Data/MessageInfo.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

class UAbilityDataAsset;
class ULevelUpDataAsset;
class UMessageInfo;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetInfoSignature, FAuraMessageInfo, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnXPChangedSignature, int32, Level, float, XPPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityDataSignature, const FAuraAbilityData&, Info);

/**
 *  Created in AuraHUD, which is only accessible by local client
 */
UCLASS()
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksDependencies() override;
	virtual void BroadcastInitialValues() override; // Initial HP, MP, XP, Abilities, ...

	UPROPERTY(BlueprintAssignable, Category = "GAS|AbilityData")
	FAbilityDataSignature AbilityDataDelegate;// Send Ability's Tags, Icons, Assets, ...
	UFUNCTION()
	void BroadcastGivenAbility(const FGameplayAbilitySpec& AbilitySpec);
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnManaChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnVitalAttributeChanged OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FMessageWidgetInfoSignature MessageWidgetInfoDelegate; // Item Pickup Message

	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnXPChangedSignature OnXPPercentChangedDelegate; // Send XP% and Level to UI
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS|WidgetData")
	TObjectPtr<UMessageInfo> MessageInfo;

	UFUNCTION()
	void BroadcastXPToUI(int32 XP, int32 Level, ULevelUpDataAsset* LevelUpDA) const;
};
