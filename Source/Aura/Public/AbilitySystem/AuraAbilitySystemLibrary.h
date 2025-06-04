// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassDataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

struct FGameplayEffectContextHandle;
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
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass = ECharacterClass::DefaultClass);


	/**
	 * Add widget to OverlayWidget -> Canvas -> Overlay_Game
	 * @param WorldContextObject 
	 * @param InNewWidget 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|UI")
	static bool AddWidgetToRootCanvasPanel(const UObject* WorldContextObject, UUserWidget* InNewWidget);

	/**
	 * Get DA_CharacterClass.
	 * Client can't get GameMode so this will always return nullptr
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|UI")
	static UCharacterClassDataAsset* GetCharacterClassDataAsset(const UObject* WorldContextObject);

#pragma region Damage
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlocked(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlocked(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlocked);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCrit(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCrit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCrit);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsStaggerDamage(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsStaggerDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContext, bool bStagger);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsShowDamageOnTarget(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsShowDamageOnTarget(UPARAM(ref) FGameplayEffectContextHandle& EffectContext, bool bShowDamageOnTarget);
#pragma endregion
};
