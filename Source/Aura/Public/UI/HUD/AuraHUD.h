// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

struct FPlayerAbilityData;
struct FGameplayAbilitySpec;
class USpellMenuWidgetController;
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
	UOverlayWidgetController* CreateOrGetOverlayWC();
	UAttributeMenuWidgetController* CreateOrGetAttributeMenuWC();
	USpellMenuWidgetController* CreateOrGetSpellMenuWC();

	void InitAuraHUD(const FWidgetControllerParams& WCParams);
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraPlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class AAuraPlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UAuraAttributeSet> AttributeSet;

	void BroadcastAllActivatableAbilities() const;

	void InitOverlay(); // Create and Set Overlay's WidgetController, then broadcast initial values
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

	UFUNCTION(BlueprintImplementableEvent)
	FText GetLockedDescription(const int32 LevelRequirement);
private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass; // For WBP_Overlay
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;

	FWidgetControllerParams GetHUDControllerParams() const;
};
