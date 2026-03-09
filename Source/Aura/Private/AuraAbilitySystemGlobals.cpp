// Copyright Hung


#include "AuraAbilitySystemGlobals.h"

#include "AuraAbilityTypes.h"
#include "AuraEffectTypes.h"
#include "Character/AuraCharacterBase.h"
#include "GameplayAbilitiesDeveloperSettings.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Cue/AuraCueManager.h"
#include "AbilitySystemLog.h"

UAuraAbilitySystemComponent* UAuraAbilitySystemGlobals::GetAuraASC(const AActor* Actor)
{
	if (const AAuraCharacterBase* AuraChara = Cast<AAuraCharacterBase>(Actor))
	{
		return AuraChara->GetAuraAbilitySystemComponent();
	}
	return nullptr;
}

FGameplayAbilityActorInfo* UAuraAbilitySystemGlobals::AllocAbilityActorInfo() const
{
	return new FAuraAbilityActorInfo();
}

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraEffectContext();
}


//Required: for UAuraCueManager (Replace UGameplayCueManager with UAuraCueManager)
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
				ABILITY_LOG(Error, TEXT("Unable to Load GameplayCueManager %s"),
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
			ABILITY_LOG(Warning, TEXT("No GameplayCueNotifyPaths were specified in DefaultGame.ini under "
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


void UAuraAbilitySystemGlobals::InitGameplayCueParameters(FGameplayCueParameters& CueParameters,
	const FGameplayEffectContextHandle& EffectContext)
{	// Super::InitGameplayCueParameters(CueParameters, EffectContext);

	if (!EffectContext.IsValid()) return;
	// CueParameters.EffectContext = EffectContext; // Copy Context over wholesale. Projects may want to override this and not copy over all data

	if (const FHitResult* HitResult = EffectContext.GetHitResult())
	{
		CueParameters.Location = HitResult->bBlockingHit ? HitResult->ImpactPoint : HitResult->TraceEnd;
		CueParameters.Normal = HitResult->ImpactNormal;
		CueParameters.PhysicalMaterial = HitResult->PhysMaterial;
	}
	else
	{
		CueParameters.Location = EffectContext.GetOrigin();
	}

	CueParameters.Instigator = EffectContext.GetInstigator();
	CueParameters.EffectCauser = EffectContext.GetEffectCauser();
	CueParameters.SourceObject = EffectContext.GetSourceObject(); // CueParameters.TargetAttachComponent;
}
