// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AuraGameplayAbility.h"
#include "Data/CharacterClassDataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilityDataAsset;
class AAuraHUD;
class USpellMenuWidgetController;
struct FAuraGameplayEffectContext;
struct FInstancedStruct;
struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
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
	// Get or Create if nullptr: AttributeMenuWidgetController from AuraHUD
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|WidgetController")
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);


	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, UObject* SourceObject,
		const ECharacterClass CharacterClass, const float Level, UAbilitySystemComponent* ASC);
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass = ECharacterClass::DefaultClass);

	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|XP")
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);


#pragma region ActorFunctions
	// Add widget to OverlayWidget -> Canvas -> Overlay_Screen
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject", ExpandBoolAsExecs="ReturnValue"), Category="AuraAbilitySystemLibrary|UI")
	static bool AddWidgetToRootCanvasPanel(const UObject* WorldContextObject, UUserWidget* InNewWidget);

	UFUNCTION(BlueprintCallable, meta=(ExpandBoolAsExecs="ReturnValue"), Category="AuraAbilitySystemLibrary|Actor")
	static bool YawActorToLocation(AActor* InActor, FVector InLocation,
		float DeltaTime, float InterpSpeed, float DegreeTolerance = 0.1); // return true if finished
	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersInRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors,
		const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin, bool bShowDebug = false);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);
#pragma endregion


	/**
	 * Get Server DA_CharacterClass, const can't be changed
	 * Client can't access GameMode so this will always return nullptr
	 */
	static const UCharacterClassDataAsset* GetGameModeCharacterClassDataAsset(const UObject* WorldContextObject);
	// Client can't get GameMode so this will always return nullptr
	static const UAbilityDataAsset* GetGameModeAbilityDataAsset(const UObject* WorldContextObject);
	
	// return nullptr for non-local or dedicated-server
	static AAuraHUD* GetAuraHUD(const UObject* WorldContextObject);


// FAuraGameplayEffectContext ========================================================================================================
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
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsStaggerDamage(UPARAM(ref) FGameplayEffectContextHandle& EffectContext, bool bStagger);
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsShowDamageOnTarget(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsShowDamageOnTarget(UPARAM(ref) FGameplayEffectContextHandle& EffectContext, bool bShowDamageOnTarget);
#pragma endregion

#pragma region InstancedStruct
	/*UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|Cue")
	static FInstancedStruct GetInstancedStruct(const FGameplayEffectContextHandle& EffectContextHandle);*/
	// Return pointer to EffectContextHandle's InstancedStruct, CANNOT send pointer with UFUNCTION()
	static FInstancedStruct* GetInstancedStructPointer(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Cue")
	static void SetInstancedStruct(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FInstancedStruct& InStruct);
#pragma endregion
};