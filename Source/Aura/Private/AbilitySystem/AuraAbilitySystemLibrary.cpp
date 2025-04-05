// Copyright Hung


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

// #include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
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
			return AuraHUD->GetOverlayWidgetController(CreateWidgetControllerParams(PC));
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
			return AuraHUD->GetAttributeMenuWidgetController(CreateWidgetControllerParams(PC));
		}
	}
	return nullptr;
}


FWidgetControllerParams UAuraAbilitySystemLibrary::CreateWidgetControllerParams(APlayerController* PC)
{
	AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	UAttributeSet* AS = PS->GetAttributeSet();
	return FWidgetControllerParams(PC, PS, ASC, AS);
}
