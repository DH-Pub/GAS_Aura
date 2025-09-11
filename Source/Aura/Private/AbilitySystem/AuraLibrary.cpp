// Copyright Hung


#include "AbilitySystem/AuraLibrary.h"

#include "AuraGameplayEffectTypes.h"
#include "AbilitySystem/Data/AbilityDataAsset.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Game/AuraGameModeBase.h"
#include "Game/AuraGameStateBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/AuraPlayerController.h"
#include "StructUtils/InstancedStruct.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* UAuraLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	/*TArray<APlayerController*> PlayerList;
	GEngine->GetAllLocalPlayerControllers(PlayerList);*/
	// UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	// WorldContextObject->GetWorld()->GetFirstPlayerController(); // ??? not consistent if server has no player
	if (const AAuraPlayerController* PC = Cast<AAuraPlayerController>(GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld())))
	{
		if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>()) return AuraHUD->CreateOrGetOverlayWC();
	}
	return nullptr;
}
UAttributeMenuWidgetController* UAuraLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	if (const AAuraPlayerController* PC = Cast<AAuraPlayerController>(GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld())))
	{
		if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>()) return AuraHUD->CreateOrGetAttributeMenuWC();
	}
	return nullptr;
}
USpellMenuWidgetController* UAuraLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	if (AAuraPlayerController* PC = Cast<AAuraPlayerController>(GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld())))
	{
		if (AAuraHUD* AuraHUD = PC->GetHUD<AAuraHUD>()) return AuraHUD->CreateOrGetSpellMenuWC();
	}
	return nullptr;
}

/* Make sure to check HasAuthority before calling this */
void UAuraLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, UObject* SourceObject,
	const ECharacterClass CharacterClass, const float Level, UAbilitySystemComponent* ASC)
{
	const UCharacterClassDataAsset* ClassData = GetGameModeCharacterClassDataAsset(WorldContextObject);
	const FCharacterClassDefaultInfo* ClassDefaultInfo = ClassData->GetClassDefaultInfo(CharacterClass);
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceObject);

	const FGameplayEffectSpecHandle PrimaryAttrSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo->PrimaryAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttrSpecHandle.Data);

	const FGameplayEffectSpecHandle SecondaryAttrSpecHandle = ASC->MakeOutgoingSpec(ClassData->SecondaryAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttrSpecHandle.Data);

	const FGameplayEffectSpecHandle VitalAttrSpecHandle = ASC->MakeOutgoingSpec(ClassData->VitalAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttrSpecHandle.Data);
}

/* Make sure to check HasAuthority() before calling this */
void UAuraLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, const ECharacterClass CharacterClass)
{
	const UCharacterClassDataAsset* ClassData = GetGameModeCharacterClassDataAsset(WorldContextObject);
	if (ClassData == nullptr) return;
	
	for (const TSubclassOf ClassAbility : ClassData->CommonAbilities)
	{
		// These abilities do not change according to levels (Eg: HitReact, ...)
		ASC->GiveAbility(FGameplayAbilitySpec(ClassAbility, 1));
	}
	
	if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
	{
		for (const TSubclassOf Ability : ClassData->GetClassDefaultInfo(CharacterClass)->ClassAbilities)
		{
			ASC->GiveAbility(FGameplayAbilitySpec(Ability, ICombatInterface::Execute_GetCharacterLevel(ASC->GetAvatarActor())));
		}
	}
}
int32 UAuraLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	const ECharacterClass CharacterClass, const int32 CharacterLevel)
{
	const UCharacterClassDataAsset* ClassData = GetGameModeCharacterClassDataAsset(WorldContextObject);
	if (ClassData == nullptr) return 0;

	const FCharacterClassDefaultInfo* Info = ClassData->GetClassDefaultInfo(CharacterClass);
	return static_cast<int32>(Info->XPReward.GetValueAtLevel(CharacterLevel));
}

bool UAuraLibrary::AddWidgetToRootCanvasPanel(const UObject* WorldContextObject, UUserWidget* InNewWidget)
{
	if (InNewWidget == nullptr) return false;
	if (const UOverlayWidgetController* OverlayWC = GetOverlayWidgetController(WorldContextObject))
	{
		if (UOverlay* Overlay = OverlayWC->Overlay_Screen) // Set inside WBP_Overlay WidgetControllerSet
		{
			Overlay->AddChildToOverlay(InNewWidget);
			return true;
		}
	}
	return false;
}

bool UAuraLibrary::YawActorToLocation(AActor* InActor, const FVector InLocation, const float DeltaTime,
	const float InterpSpeed, const float DegreeTolerance)
{
	const FRotator CurrentRot = InActor->GetActorRotation();
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(InActor->GetActorLocation(), InLocation);
	LookAtRot.Pitch = LookAtRot.Roll = 0.f;
	if (UKismetMathLibrary::Abs(LookAtRot.Yaw - CurrentRot.Yaw) < DegreeTolerance) return true; // if rotation is within Tolerance
	
	const FRotator InterpToRot = UKismetMathLibrary::RInterpTo_Constant(CurrentRot,LookAtRot, DeltaTime, InterpSpeed);
	InActor->GetRootComponent()->SetWorldRotation(FRotator(CurrentRot.Pitch, InterpToRot.Yaw, CurrentRot.Roll));
	return false;
}

void UAuraLibrary::GetLivePlayersInRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors,
	const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin, bool bShowDebug)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams SphereParams;
		SphereParams.AddIgnoredActors(ActorsToIgnore);
		World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutActors.AddUnique(Overlap.GetActor());
			}
		}

		if (bShowDebug)
		{
			UKismetSystemLibrary::DrawDebugSphere(WorldContextObject, Origin, Radius, 12, FColor::Red, 1.f);
		}
	}
}

bool UAuraLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	if (FirstActor == nullptr || SecondActor == nullptr) return false;
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return !(bBothArePlayers || bBothAreEnemies);
}


const UAbilityDataAsset* UAuraLibrary::GetAbilityDataAsset(const UObject* WorldContextObject)
{
	if (AAuraGameStateBase* GameState = Cast<AAuraGameStateBase>(UGameplayStatics::GetGameState(WorldContextObject)))
	{
		return GameState->AbilityData;
	}
	return nullptr;
}
const FAuraAbilityData* UAuraLibrary::FindAbilityDataByTag(const UObject* WorldContextObject, const FGameplayTag& AbilityTag)
{
	if (const AAuraGameStateBase* GameState = Cast<AAuraGameStateBase>(UGameplayStatics::GetGameState(WorldContextObject)))
	{
		for (const FAuraAbilityData& Data : GameState->AbilityData->AbilityDataList)
		{
			if (AbilityTag.MatchesTagExact(Data.AbilityTag)) return &Data;
		}
	}
	// UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag on List [%s]"), *GetNameSafe(this));
	return nullptr;
}
const FAuraAbilityData* UAuraLibrary::FindAbilityDataByTags(const UObject* WorldContextObject,
	const FGameplayTagContainer& AssetTags)
{
	if (const AAuraGameStateBase* GameState = Cast<AAuraGameStateBase>(UGameplayStatics::GetGameState(WorldContextObject)))
	{
		for (const FAuraAbilityData& Data : GameState->AbilityData->AbilityDataList)
		{
			if (AssetTags.HasTagExact(Data.AbilityTag)) return &Data;
		}
	}
	return nullptr;
}

const UCharacterClassDataAsset* UAuraLibrary::GetGameModeCharacterClassDataAsset(const UObject* WorldContextObject)
{
	if (const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameMode->CharacterClassData;
	}
	return nullptr;
}

AAuraHUD* UAuraLibrary::GetAuraHUD(const UObject* WorldContextObject)
{
	return GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld())->GetHUD<AAuraHUD>();
}

/*
 * =============== FAuraGameplayEffectContext ========================================================================================================================
 */
#pragma region Damage
void UAuraLibrary::SetIsStaggerDamage(FGameplayEffectContextHandle& EffectContext, const bool bValue)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContext.Get()))
	{
		AuraEffectContext->SetIsStagger(bValue);
	}
}
void UAuraLibrary::SetIsShowDamageOnTarget(FGameplayEffectContextHandle& EffectContext, const bool bValue)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContext.Get()))
	{
		AuraEffectContext->bShowDamageOnTarget = bValue;
	}
}
#pragma endregion


/*
 * ==================================================================================================================================
 */
#pragma region InstancedStruct
FInstancedStruct* UAuraLibrary::GetInstancedStructPointer(
	const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext =  static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->GetInstancedStruct();
	}
	return nullptr;
}
void UAuraLibrary::SetInstancedStruct(FGameplayEffectContextHandle& EffectContextHandle,
	const FInstancedStruct& InStruct)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetInstancedStruct(InStruct);
	}
}
#pragma endregion
