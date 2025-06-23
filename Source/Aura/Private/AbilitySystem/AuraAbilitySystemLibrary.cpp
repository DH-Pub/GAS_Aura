// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AuraGameplayEffectTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	// GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld());
	// UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	// WorldContextObject->GetWorld()->GetFirstPlayerController(); // ??? not consistent
	
	if (APlayerController* PC = GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			return AuraHUD->GetOverlayWidgetController(FWidgetControllerParams(PC, PS, ASC, AS));
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			return AuraHUD->GetAttributeMenuWidgetController(FWidgetControllerParams(PC, PS, ASC, AS));
		}
	}
	return nullptr;
}

/* Make sure to check HasAuthority before calling this */
void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, UObject* SourceObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	UCharacterClassDataAsset* ClassData = GetCharacterClassDataAsset(WorldContextObject);
	const FCharacterClassDefaultInfo ClassDefaultInfo = ClassData->GetClassDefaultInfo(CharacterClass);
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceObject);

	const FGameplayEffectSpecHandle PrimaryAttrSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttrSpecHandle.Data);

	const FGameplayEffectSpecHandle SecondaryAttrSpecHandle = ASC->MakeOutgoingSpec(ClassData->SecondaryAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttrSpecHandle.Data);

	const FGameplayEffectSpecHandle VitalAttrSpecHandle = ASC->MakeOutgoingSpec(ClassData->VitalAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttrSpecHandle.Data);
}

/* Make sure to check HasAuthority() before calling this */
void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, const ECharacterClass CharacterClass)
{
	UCharacterClassDataAsset* ClassData = GetCharacterClassDataAsset(WorldContextObject);
	if (ClassData == nullptr) return;
	
	for (const TSubclassOf ClassAbility : ClassData->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec(ClassAbility, 1); // These abilities do not change according to levels 
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo ClassDefaultInfo = ClassData->GetClassDefaultInfo(CharacterClass);
	
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(ASC->GetAvatarActor()))
	{
		for (TSubclassOf Ability : ClassDefaultInfo.ClassAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(Ability, CombatInterface->GetCharacterLevel());
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemLibrary::AddWidgetToRootCanvasPanel(const UObject* WorldContextObject, UUserWidget* InNewWidget, TEnumAsByte<EOutcome>& Outcome)
{
	Outcome = EOutcome::Failure;
	if (InNewWidget == nullptr) return;
	if (const APlayerController* PC = GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			const UAuraUserWidget* RootOverlay = AuraHUD->GetOverlayWidget();
			if (UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(RootOverlay->GetRootWidget()))
			{
				if (UOverlay* OverlayGame = Cast<UOverlay>(CanvasPanel->GetChildAt(0)))
				{
					OverlayGame->AddChildToOverlay(InNewWidget);
				}
				// CanvasPanel->AddChild(InNewWidget);
				Outcome = EOutcome::Success;
			}
		}
	}
}

void UAuraAbilitySystemLibrary::YawActorToLocation(TEnumAsByte<EOutcome>& Outcome, AActor* InActor, const FVector InLocation,
	const float DeltaTime, const float InterpSpeed, const float DegreeTolerance)
{
	const FRotator CurrentRot = InActor->GetActorRotation();
	const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(InActor->GetActorLocation(), InLocation);
	if (UKismetMathLibrary::Abs(LookAtRot.Yaw - CurrentRot.Yaw) < DegreeTolerance)
	{
		Outcome = Success; return;
	}
	const FRotator InterpToRot = UKismetMathLibrary::RInterpTo_Constant(CurrentRot,LookAtRot, DeltaTime, InterpSpeed);
	InActor->GetRootComponent()->SetWorldRotation(FRotator(CurrentRot.Pitch, InterpToRot.Yaw, CurrentRot.Roll));
	Outcome = Failure;
}

void UAuraAbilitySystemLibrary::GetLivePlayersInRadius(const UObject* WorldContextObject, TArray<AActor*>& OutActors,
	const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity,
			FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
			FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				// OutActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
				OutActors.AddUnique(Overlap.GetActor());
			}
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	return !(bBothArePlayers || bBothAreEnemies);
}

UCharacterClassDataAsset* UAuraAbilitySystemLibrary::GetCharacterClassDataAsset(const UObject* WorldContextObject)
{
	if (const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameMode->CharacterClassData;
	}
	return nullptr;
}


/*
 * =======================================================================================================================================
 */
#pragma region Damage
bool UAuraAbilitySystemLibrary::IsBlocked(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext =  static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsBlocked();
	}
	return false;
}
void UAuraAbilitySystemLibrary::SetIsBlocked(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlocked)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsBlocked(bInIsBlocked);
	}
}


bool UAuraAbilitySystemLibrary::IsCrit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext =  static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsCrit();
	}
	return false;
}
void UAuraAbilitySystemLibrary::SetIsCrit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCrit)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->SetIsCrit(bInIsCrit);
	}
}


bool UAuraAbilitySystemLibrary::IsStaggerDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext =  static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->IsStagger();
	}
	return false;
}
void UAuraAbilitySystemLibrary::SetIsStaggerDamage(FGameplayEffectContextHandle& EffectContext, bool bStagger)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContext.Get()))
	{
		AuraEffectContext->SetIsStagger(bStagger);
	}
}


bool UAuraAbilitySystemLibrary::IsShowDamageOnTarget(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext =  static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->bShowDamageOnTarget;
	}
	return false;
}
void UAuraAbilitySystemLibrary::SetIsShowDamageOnTarget(FGameplayEffectContextHandle& EffectContext, const bool bShowDamageOnTarget)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContext.Get()))
	{
		AuraEffectContext->bShowDamageOnTarget = bShowDamageOnTarget;
	}
}
#pragma endregion


/*
 * ==================================================================================================================================
 */
#pragma region GameplayCue
TArray<FVector_NetQuantize> UAuraAbilitySystemLibrary::GetCueLocations(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext =  static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraEffectContext->CueLocations;
	}
	return TArray<FVector_NetQuantize>();
}
void UAuraAbilitySystemLibrary::SetCueLocations(FGameplayEffectContextHandle& EffectContextHandle, const TArray<FVector_NetQuantize> InLocations)
{
	if (FAuraGameplayEffectContext* AuraEffectContext =  static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraEffectContext->CueLocations = InLocations;
	}
}
#pragma endregion
