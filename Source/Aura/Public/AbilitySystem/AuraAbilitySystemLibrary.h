// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AuraGameplayAbility.h"
#include "Data/CharacterClassDataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

struct FAuraGameplayEffectContext;
struct FInstancedStruct;
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

	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category = "AuraAbilitySystemLibrary|XP")
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);
	
#pragma region ActorFunctions
	/**
	 * Add widget to OverlayWidget -> Canvas -> Overlay_Game
	 * @param WorldContextObject 
	 * @param InNewWidget 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject", ExpandEnumAsExecs="Outcome"), Category="AuraAbilitySystemLibrary|UI")
	static void AddWidgetToRootCanvasPanel(const UObject* WorldContextObject, UUserWidget* InNewWidget, TEnumAsByte<EOutcome>& Outcome);

	UFUNCTION(BlueprintCallable, meta=(ExpandEnumAsExecs="Outcome"), Category="AuraAbilitySystemLibrary|Actor")
	static void YawActorToLocation(TEnumAsByte<EOutcome>& Outcome, AActor* InActor, FVector InLocation,
		float DeltaTime, float InterpSpeed, float DegreeTolerance = 0.1);
	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersInRadius(const UObject* WorldContextObject,
		TArray<AActor*>& OutActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin);
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);
#pragma endregion

	
	/**
	 * Get DA_CharacterClass.
	 * Client can't get GameMode so this will always return nullptr
	 */
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject"), Category="AuraAbilitySystemLibrary|UI")
	static UCharacterClassDataAsset* GetCharacterClassDataAsset(const UObject* WorldContextObject);


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
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|Cue")
	static FInstancedStruct GetInstancedStruct(const FGameplayEffectContextHandle& EffectContextHandle);
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|Cue")
	static void SetInstancedStruct(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const FInstancedStruct& InStruct);
#pragma endregion
};