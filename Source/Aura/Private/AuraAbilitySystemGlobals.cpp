// Copyright Hung


#include "AuraAbilitySystemGlobals.h"

#include "AuraCueManager.h"
#include "AuraEffectTypes.h"
#include "Character/AuraCharacterBase.h"
#include "GameplayAbilitiesDeveloperSettings.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

UAuraAbilitySystemComponent* UAuraAbilitySystemGlobals::GetAuraASC(const AActor* Actor)
{
	if (const AAuraCharacterBase* AuraChara = Cast<AAuraCharacterBase>(Actor))
	{
		return AuraChara->GetAuraAbilitySystemComponent();
	}
	return nullptr;
}

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const {return new FAuraEffectContext();}

UGameplayCueManager* UAuraAbilitySystemGlobals::GetGameplayCueManager()
{
	if (GlobalGameplayCueManager == nullptr)
	{
		const UGameplayAbilitiesDeveloperSettings* DeveloperSettings = GetDefault<UGameplayAbilitiesDeveloperSettings>();
		if (GlobalGameplayCueManager == nullptr && DeveloperSettings->GlobalGameplayCueManagerName.IsValid())
		{	// Loads mud specific GameplayCueManager object if specified
			GlobalGameplayCueManager = LoadObject<UAuraCueManager>(nullptr,
				*DeveloperSettings->GlobalGameplayCueManagerName.ToString(),
				nullptr, LOAD_None, nullptr);
			if (GlobalGameplayCueManager == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Unable to Load GameplayCueManager %s"),
					*DeveloperSettings->GlobalGameplayCueManagerName.ToString());
			}
		}

		// Load specific GameplayCueManager class if specified
		if ( GlobalGameplayCueManager == nullptr && DeveloperSettings->GlobalGameplayCueManagerClass.IsValid() )
		{
			const UClass* GCMClass = LoadClass<UObject>(nullptr,
				*DeveloperSettings->GlobalGameplayCueManagerClass.ToString(),
				nullptr, LOAD_None, nullptr);
			if (GCMClass) GlobalGameplayCueManager = NewObject<UAuraCueManager>(this, GCMClass);
		}

		if ( GlobalGameplayCueManager == nullptr)
		{	// Fallback to base native class
			GlobalGameplayCueManager = NewObject<UAuraCueManager>(this, UAuraCueManager::StaticClass());
		}

		GlobalGameplayCueManager->OnCreated();

		if (GetGameplayCueNotifyPaths().IsEmpty())
		{
			AddGameplayCueNotifyPath(TEXT("/Game"));
			UE_LOG(LogTemp, Warning, TEXT("No GameplayCueNotifyPaths were specified in DefaultGame.ini under "
				"[/Script/GameplayAbilities.AbilitySystemGlobals]. Falling back to using all of /Game/. "
				"This may be slow on large projects. Consider specifying which paths are to be searched."));
		}

		if (GlobalGameplayCueManager->ShouldAsyncLoadObjectLibrariesAtStart())
		{
			StartAsyncLoadingObjectLibraries();
		}
	}

	check(GlobalGameplayCueManager);
	return GlobalGameplayCueManager;
}
