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
	/**
	 * Bind callbacks if not yet and return controller
	 * @param InASC
	 * @param bASCCanBeNull if true, all Controller can be set to nullptr
	 */
	void InitAuraHUD(class UAuraAbilitySystemComponent* InASC, const bool bASCCanBeNull = false);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UAuraPrimaryLayout> OverallLayout; // Set when Creating UAuraPrimaryLayout
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TSubclassOf<UAuraPrimaryLayout> OverallLayoutClass;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnWidgetControllersSet();

	/*UE_DEPRECATED(all, "Using MessageDataTable but this can be used")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Overlay")
	TObjectPtr<class UMessageInfo> MessageInfo; // This is DEPRECATED, use MessageDataTable
	UPROPERTY(EditDefaultsOnly, Category="Aura|Overlay")
	TObjectPtr<UDataTable> MessageDataTable;*/


	UPROPERTY(EditDefaultsOnly, Category="Aura")
	TObjectPtr<const class UAttributeDataAsset> AttributeData;
	const TMap<struct FGameplayTag, struct FAuraAttributeData>& GetAttributeDataList() const;

	UFUNCTION(BlueprintImplementableEvent)
	FText GetLockedDescription(const int32 LevelRequirement);


#pragma region UIFunctions
	// HUD exist on local only so this return nullptr others
	static AAuraHUD* Get(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject", HidePin="WorldContextObject", CompactNodeTitle="GameHUD_WC"))
	static UOverlayWidgetController* GetGameHUDController(const UObject* WorldContextObject)
	{
		if (AAuraHUD* HUD = Get(WorldContextObject)) return HUD->OverlayController;
		return nullptr;
	}
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject", HidePin="WorldContextObject", CompactNodeTitle="AttributeWC"))
	static UAttributeMenuWidgetController* GetAttributeMenuController(const UObject* WorldContextObject)
	{
		if (AAuraHUD* HUD = Get(WorldContextObject)) return HUD->AttributeMenuController;
		return nullptr;
	}
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject", HidePin="WorldContextObject", CompactNodeTitle="SpellWC"))
	static USpellMenuWidgetController* GetSpellMenuController(const UObject* WorldContextObject)
	{
		if (AAuraHUD* HUD = Get(WorldContextObject)) return HUD->SpellMenuController;
		return nullptr;
	}

	// Add widget to OverlayWidget -> Canvas -> Overlay_Screen
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject", ExpandBoolAsExecs="ReturnValue"), Category="AuraLibrary|UI")
	static bool AddWidgetToRootCanvasPanel(UUserWidget* InNewWidget);
#pragma endregion

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY() // Overlay is always available with this
	TObjectPtr<UOverlayWidgetController> OverlayController;
	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuController; // Widget created in OverlayWidget BP
	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuController;
};
