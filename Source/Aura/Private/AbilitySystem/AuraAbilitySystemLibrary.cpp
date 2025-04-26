// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

// #include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, UObject* SourceObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	if (UCharacterClassDataAsset* ClassData = GetCharacterClassDataAsset(WorldContextObject))
	{
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
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	if (UCharacterClassDataAsset* ClassData = GetCharacterClassDataAsset(WorldContextObject))
	{
		for (const TSubclassOf ClassAbility : ClassData->CommonAbilities)
		{
			FGameplayAbilitySpec AbilitySpec(ClassAbility, 1);
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

bool UAuraAbilitySystemLibrary::AddWidgetToRootCanvasPanel(const UObject* WorldContextObject, UUserWidget* InNewWidget)
{
	if (const APlayerController* PC = GEngine->GetFirstLocalPlayerController(WorldContextObject->GetWorld()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			const UAuraUserWidget* RootOverlay = AuraHUD->GetOverlayWidget();
			if (UCanvasPanel* CanvasPanel = Cast<UCanvasPanel>(RootOverlay->GetRootWidget()))
			{
				CanvasPanel->AddChild(InNewWidget);
				return true;
			}
		}
	}
	return false;
}

UCharacterClassDataAsset* UAuraAbilitySystemLibrary::GetCharacterClassDataAsset(const UObject* WorldContextObject)
{
	if (const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameMode->CharacterClassData;
	}
	return nullptr;
}
