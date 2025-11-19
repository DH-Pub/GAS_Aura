// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class USpellMenuWidgetController;
class UAttributeMenuWidgetController;
class UAuraUserWidget;
class UOverlayWidgetController;

/**
 *
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	/** Bind callbacks if not yet and return controller */
	void InitAuraHUD(class AAuraPlayerController* PC, class AAuraPlayerState* PS, class AAuraCharacterBase* Character);
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraPlayerController> PlayerController;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AAuraPlayerState> PlayerState;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UAuraAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UAuraAttributeSet> AttributeSet;

	UPROPERTY()
	TObjectPtr<UAuraUserWidget> OverlayWidget;

	UPROPERTY() // Overlay is always available with this
	TObjectPtr<UOverlayWidgetController> OverlayController;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuController; // Widget created in OverlayWidget BP
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USpellMenuWidgetController> SpellMenuController;

	UPROPERTY(EditDefaultsOnly, Category="Default")
	TObjectPtr<class UAttributeDataAsset> AttributeData;
	const TMap<struct FGameplayTag, struct FAuraAttributeData>& GetAttributeDataList() const;

	UFUNCTION(BlueprintImplementableEvent)
	FText GetLockedDescription(const int32 LevelRequirement);


#pragma region UIFunctions
	// HUD exist on local only so this return nullptr others
	static AAuraHUD* GetAuraHUD(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject", HidePin="WorldContextObject", CompactNodeTitle="AttributeWC"))
	static UAttributeMenuWidgetController* GetAttributeMenuController(const UObject* WorldContextObject)
	{
		if (AAuraHUD* HUD = GetAuraHUD(WorldContextObject)) return HUD->AttributeMenuController;
		return nullptr;
	}
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject", HidePin="WorldContextObject", CompactNodeTitle="SpellWC"))
	static USpellMenuWidgetController* GetSpellMenuController(const UObject* WorldContextObject)
	{
		if (AAuraHUD* HUD = GetAuraHUD(WorldContextObject)) return HUD->SpellMenuController;
		return nullptr;
	}

	// Add widget to OverlayWidget -> Canvas -> Overlay_Screen
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject", ExpandBoolAsExecs="ReturnValue"), Category="AuraLibrary|UI")
	static bool AddWidgetToRootCanvasPanel(UUserWidget* InNewWidget);
#pragma endregion


private:
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UAuraUserWidget> OverlayWidgetClass; // For WBP_Overlay
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
};
