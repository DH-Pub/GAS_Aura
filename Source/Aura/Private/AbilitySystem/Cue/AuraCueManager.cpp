// Copyright Hung


#include "AbilitySystem/Cue/AuraCueManager.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameFramework/PlayerState.h"

bool EnableSuppressCuesOnGameplayCueManager = true;
static FAutoConsoleVariableRef CVarEnableSuppressCuesOnGameplayCueManager(
	TEXT("AbilitySystem.GameplayCue.EnableSuppressCuesOnGameplayCueManager"),
	EnableSuppressCuesOnGameplayCueManager, TEXT("Allows the GameplayCueManager to suppress cues when "
	"the bSuppressGameplayCues is set on the target AbilitySystemComponent"), ECVF_Default );

void UAuraCueManager::InvokeGameplayCueExecuted_FromSpec(UAbilitySystemComponent* OwningComponent,
	const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey)
{	// Super::InvokeGameplayCueExecuted_FromSpec(OwningComponent, Spec, PredictionKey);
	if (Spec.Def->GameplayCues.Num() == 0)
	{	// This spec doesn't have any GCs, so early out
		ABILITY_LOG(Verbose, TEXT("No GCs in this Spec, so early out: %s"), *Spec.Def->GetName());
		return;
	}

	if (EnableSuppressCuesOnGameplayCueManager && OwningComponent && OwningComponent->bSuppressGameplayCues) return;

	FGameplayCuePendingExecute PendingCue;
	// Transform the GE Spec into GameplayCue parameters here (on the server)
	PendingCue.PayloadType = EGameplayCuePayloadType::CueParameters;
	PendingCue.OwningComponent = OwningComponent;
	PendingCue.PredictionKey = PredictionKey;

	for (const FGameplayEffectCue& EffectCue : Spec.Def->GameplayCues)
	{	// Add all GameplayCue Tags from the GE into the GameplayCueTags PendingCue.list
		for (const FGameplayTag& Tag: EffectCue.GameplayCueTags)
		{
			if (Tag.IsValid()) PendingCue.GameplayCueTags.AddUnique(Tag);
		}
	}

	if (PendingCue.GameplayCueTags.Num() == 0)
	{
		ABILITY_LOG(Warning, TEXT("GE %s has GameplayCues but not valid GameplayCue tag."), *Spec.Def->GetName());
		return;
	}

	UAbilitySystemGlobals::Get().InitGameplayCueParameters_GESpec(PendingCue.CueParameters, Spec);
	if (Spec.Period > 0.f)
	{	// Update Location for everytime the Periodical Effect ex
		PendingCue.CueParameters.Location = OwningComponent->GetAvatarActor()->GetActorLocation();
	}

	/* AddPendingCueExecuteInternal(PendingCue); */
	if (ProcessPendingCueExecute(PendingCue)) PendingExecuteCues.Add(PendingCue);
	if (GameplayCueSendContextCount == 0)
	{	// Not in a context, flush now
		FlushPendingCues();
	}
}

void UAuraCueManager::HandleGameplayCue(AActor* TargetActor, FGameplayTag GameplayCueTag,
	EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters, EGameplayCueExecutionOptions Options)
{
	switch (EventType)
	{
	case EGameplayCueEvent::WhileActive:
		if (APlayerState* PS = Cast<APlayerState>(TargetActor))
		{
			FHandleGameplayCue& Cue = HandleCues.Add_GetRef(
				FHandleGameplayCue(GameplayCueTag, EventType, Parameters, Options));
			Cue.PlayerState = PS;
			if (!PS->OnPawnSet.IsAlreadyBound(this, &UAuraCueManager::CueSetHandleGameplayCue))
			{
				PS->OnPawnSet.AddDynamic(this, &UAuraCueManager::CueSetHandleGameplayCue);
			}
		}
		else Super::HandleGameplayCue(TargetActor, GameplayCueTag, EventType, Parameters, Options);
		break;
	default:
		Super::HandleGameplayCue(TargetActor, GameplayCueTag, EventType, Parameters, Options);
		break;
	}
}

void UAuraCueManager::CueSetHandleGameplayCue(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	Player->OnPawnSet.RemoveAll(this);

	for (int32 i = HandleCues.Num() - 1; i >= 0; --i) // Iterate backwards so we can remove during loop
	{
		FHandleGameplayCue& Cue = HandleCues[i];
		if (Cue.PlayerState != Player) continue;
		Super::HandleGameplayCue(NewPawn, Cue.GameplayCueTag, Cue.EventType, Cue.Parameters, Cue.Options);
		HandleCues.RemoveAt(i);
	}
}
