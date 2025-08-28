// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

struct FGameplayAbilitySpec;
class USpellMenuWidgetController;
class UAttributeMenuWidgetController;
class UAuraUserWidget;
class UOverlayWidgetController;

struct FWidgetControllerParams;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityDataSignature, const FAuraAbilityData&, Info);
/**
 * 
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	/** Bind callbacks if not yet and return controller */
	UOverlayWidgetController* CreateOrGetOverlayWC(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* CreateOrGetAttributeMenuWC(const FWidgetControllerParams& WCParams);
	USpellMenuWidgetController* CreateOrGetSpellMenuWC(const FWidgetControllerParams& WCParams);

	void InitAuraHUD(const FWidgetControllerParams& WCParams);
	void BroadcastAllActivatableAbilities();
	void BroadcastGivenAbility(const FGameplayAbilitySpec& Spec);
	// Create and Set Overlay's WidgetController, then broadcast initial values
	void InitOverlay(const FWidgetControllerParams& WCParams);
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController; // Widget created in OverlayWidget BP
	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<class UAttributeDataAsset> AttributeData;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<class UAbilityDataAsset> AbilityData;
	UPROPERTY(BlueprintAssignable, Category = "GAS|AbilityData")
	FAbilityDataSignature AbilityDataDelegate;// Send AbilityData (Icon, Tag, ...)

	UFUNCTION(BlueprintImplementableEvent)
	FText GetLockedDescription(const int32 LevelRequirement);
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass; // For OverlayWidget

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
};
