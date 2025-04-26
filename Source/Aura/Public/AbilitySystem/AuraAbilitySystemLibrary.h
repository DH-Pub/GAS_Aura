// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassDataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAuraUserWidget;
class UAuraWorldUserWidget;
class UAbilitySystemComponent;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
/**
 * For calling static Blueprint Functions
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// meta=(WorldContext="WorldContextObject") 
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, UObject* SourceObject,
		ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|UI")
	static bool AddWidgetToRootCanvasPanel(const UObject* WorldContextObject, UUserWidget* InNewWidget);

	/**
	 * Get DA_CharacterClass
	 * Client can't get GameMode so this will always return nullptr
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|UI")
	static UCharacterClassDataAsset* GetCharacterClassDataAsset(const UObject* WorldContextObject);
};
