// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAttributeDataAsset;
class UAbilityDataAsset;
class USpellMenuWidgetController;
class AAuraPlayerController;
class UAttributeMenuWidgetController;
class UAuraUserWidget;
class UOverlayWidgetController;

struct FWidgetControllerParams;

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

	// Create and Set Overlay's WidgetController, then broadcast initial values
	void InitOverlay(const FWidgetControllerParams& WCParams);
	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UAttributeDataAsset> AttributeData;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<UAbilityDataAsset> AbilityData;
private:
	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass; // For OverlayWidget
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	// Widget created in OverlayWidget BP
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;
	UPROPERTY(EditAnywhere, Category="Default")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
};
