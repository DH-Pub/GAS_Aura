// Copyright Hung


#include "AuraCueManager.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraEffectTypes.h"

void UAuraCueManager::FlushPendingCues()
{	// Super::FlushPendingCues();
	if (PendingExecuteCues.Num() == 0) return;
	OnFlushPendingCues.Broadcast();

	FGameplayCuePendingExecute BatchExecute;
	FAuraEffectContext* BatchContext = nullptr;
	for (int32 i = 0; i < PendingExecuteCues.Num(); i++) // Includes the FirstCueContext
	{	// Batches the rest of the PendingExecuteCues into the first's EffectContext
		FGameplayCuePendingExecute& PendingCue = PendingExecuteCues[i];
		FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(PendingCue.CueParameters.EffectContext);

		if (PendingCue.OwningComponent == nullptr) continue; // Our component may have gone away
		const bool bHasAuthority = PendingCue.OwningComponent->IsOwnerActorAuthoritative();
		const bool bLocalPredictionKey = PendingCue.PredictionKey.IsLocalClientKey();
		IAbilitySystemReplicationProxyInterface* RepInterface = PendingCue.OwningComponent->GetReplicationInterface();
		if (RepInterface == nullptr) continue; // "we are replicating through a proxy and have no avatar", so skip
		if (PendingCue.GameplayCueTags.Num() == 0) continue;
		if (bHasAuthority)
		{
			if (BatchExecute.OwningComponent)
			{
				if (AuraContext == BatchContext) continue;
				if (AuraContext) AuraContext->GetCueParamsBatched().Empty();
				// Inside BatchCuesParams will also check if AuraContext != BatchContext before adding
				BatchContext->BatchCuesParams(PendingCue.GameplayCueTags[0], PendingCue.CueParameters);
			}
			else
			{	// if BatchExecute has not been set, do it for the first time
				BatchExecute = PendingCue;
				BatchContext = FAuraEffectContext::ExtractAuraContext(BatchExecute.CueParameters.EffectContext);
				if (BatchContext) BatchContext->GetCueParamsBatched().Empty();
			}
		}
		else if (bLocalPredictionKey)
		{
			if (AuraContext)
			{
				for (const FEffectCues& EffectCues : AuraContext->GetEffectCuesList())
				{
					PendingCue.CueParameters.RawMagnitude = EffectCues.RawMagnitude;
					PendingCue.OwningComponent->InvokeGameplayCueEvent(EffectCues.CueTag,
						EGameplayCueEvent::Executed, PendingCue.CueParameters);
				}
			}
			else PendingCue.OwningComponent->InvokeGameplayCueEvent(PendingCue.GameplayCueTags[0],
				EGameplayCueEvent::Executed, PendingCue.CueParameters);
		}
	}
	PendingExecuteCues.Empty();

	if (BatchExecute.OwningComponent == nullptr) return; // BatchExecute is only set if there is a case of bHasAuthority
	IAbilitySystemReplicationProxyInterface* RepInterface = BatchExecute.OwningComponent->GetReplicationInterface();
	if (RepInterface == nullptr || BatchExecute.GameplayCueTags.Num() == 0) return;
	RepInterface->ForceReplication();
	RepInterface->NetMulticast_InvokeGameplayCueExecuted_WithParams(BatchExecute.GameplayCueTags[0],
		BatchExecute.PredictionKey, BatchExecute.CueParameters);
	CheckForTooManyRPCs(TEXT("NetMulticast_InvokeGameplayCueExecuted_WithParams"),
		BatchExecute, BatchExecute.GameplayCueTags[0].ToString(), nullptr);
}


bool EnableSuppressCuesOnGameplayCueManager = true;
static FAutoConsoleVariableRef CVarEnableSuppressCuesOnGameplayCueManager(
	TEXT("AbilitySystem.GameplayCue.EnableSuppressCuesOnGameplayCueManager"),
	EnableSuppressCuesOnGameplayCueManager, TEXT("Allows the GameplayCueManager to suppress cues when "
	"the bSuppressGameplayCues is set on the target AbilitySystemComponent"), ECVF_Default );
void UAuraCueManager::InvokeGameplayCueExecuted_FromSpec(UAbilitySystemComponent* OwningComponent,
	const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey)
{	//Super::InvokeGameplayCueExecuted_FromSpec(OwningComponent, Spec, PredictionKey);
	if (Spec.Def->GameplayCues.Num() == 0)
	{	// This spec doesn't have any GCs, so early out
		UE_LOG(LogTemp, Verbose, TEXT("No GCs in this Spec, so early out: %s"), *Spec.Def->GetName());
		return;
	}
	if (EnableSuppressCuesOnGameplayCueManager && OwningComponent && OwningComponent->bSuppressGameplayCues) return;

	FGameplayCuePendingExecute PendingCue; // Transform the GE Spec into GameplayCue parameters here (on the server)
	PendingCue.PayloadType = EGameplayCuePayloadType::CueParameters;
	PendingCue.OwningComponent = OwningComponent;
	PendingCue.PredictionKey = PredictionKey;
	const FGameplayEffectContextHandle& ContextHandle = Spec.GetEffectContext();
	PendingCue.CueParameters.Location = ContextHandle.HasOrigin() ? ContextHandle.GetOrigin() :
		ContextHandle.GetEffectCauser()->GetActorLocation();

	// UAbilitySystemGlobals::Get().InitGameplayCueParameters_GESpec(PendingCue.CueParameters, Spec);
	FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(ContextHandle);
	AuraContext->GetEffectCuesList().Empty();
	for (const FGameplayEffectCue& CueDef : Spec.Def->GameplayCues)
	{	/*for (const auto& Tag: CueDef.GameplayCueTags) if (Tag.IsValid())PendingCue.GameplayCueTags.Add(Tag);*/
		if (!CueDef.MagnitudeAttribute.IsValid()) continue;
		const FGameplayTag& Tag = CueDef.GameplayCueTags.GetByIndex(0); // we will only use the FIRST Cue Tag
		if (!Tag.IsValid()) continue;
		for (const FGameplayEffectModifiedAttribute& ModifiedAttr : Spec.ModifiedAttributes)
		{	// Loop through UGameplayEffect's Modifiers(CalculateBaseMagnitude) and/or Executions(OutExecutionOutput)
			if (ModifiedAttr.Attribute != CueDef.MagnitudeAttribute) continue; // for comparision
			// PendingCue.CueParameters.RawMagnitude = ModifiedAttr.TotalMagnitude; // FCoreEffectCues instead of this
			PendingCue.GameplayCueTags.Add(Tag);
			AuraContext->AddToEffectCuesList(Tag, ModifiedAttr.TotalMagnitude);
			break;
		}
	}
	if (PendingCue.GameplayCueTags.Num() == 0) return; // After loop, check for valid tag

	if (ContextHandle.IsValid()) PendingCue.CueParameters.EffectContext = ContextHandle;

	AddPendingCueNextTick(PendingCue); // AddPendingCueExecuteInternal(PendingCue)
}
void UAuraCueManager::InvokeGameplayCueExecuted(UAbilitySystemComponent* OwningComponent,
	const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayEffectContextHandle EffectContext)
{
	if (!GameplayCueTag.IsValid()) return;
	if (EnableSuppressCuesOnGameplayCueManager && OwningComponent && OwningComponent->bSuppressGameplayCues) return;

	if (OwningComponent)
	{
		FGameplayCuePendingExecute PendingCue;
		PendingCue.PayloadType = EGameplayCuePayloadType::CueParameters;
		PendingCue.GameplayCueTags.Add(GameplayCueTag);
		PendingCue.OwningComponent = OwningComponent;
		UAbilitySystemGlobals::Get().InitGameplayCueParameters(PendingCue.CueParameters, EffectContext);
		PendingCue.PredictionKey = PredictionKey;

		AddPendingCueNextTick(PendingCue);
	}
}
void UAuraCueManager::InvokeGameplayCueExecuted_WithParams(UAbilitySystemComponent* OwningComponent,
	const FGameplayTag GameplayCueTag, FPredictionKey PredictionKey, FGameplayCueParameters GameplayCueParameters)
{
	if (!GameplayCueTag.IsValid())return;
	if (EnableSuppressCuesOnGameplayCueManager && OwningComponent && OwningComponent->bSuppressGameplayCues) return;

	if (OwningComponent)
	{
		FGameplayCuePendingExecute PendingCue;
		PendingCue.PayloadType = EGameplayCuePayloadType::CueParameters;
		PendingCue.GameplayCueTags.Add(GameplayCueTag);
		PendingCue.OwningComponent = OwningComponent;
		PendingCue.CueParameters = GameplayCueParameters;
		PendingCue.PredictionKey = PredictionKey;

		// AddPendingCueExecuteInternal(PendingCue);
		AddPendingCueNextTick(PendingCue);
	}
}

void UAuraCueManager::InvokeGameplayCueAddedAndWhileActive_FromSpec(UAbilitySystemComponent* OwningComponent,
	const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey)
{	//Super::InvokeGameplayCueAddedAndWhileActive_FromSpec(OwningComponent, Spec, PredictionKey);
	if (Spec.Def->GameplayCues.Num() == 0) return;
	if (EnableSuppressCuesOnGameplayCueManager && OwningComponent && OwningComponent->bSuppressGameplayCues) return;

	IAbilitySystemReplicationProxyInterface* ReplicationInterface = OwningComponent->GetReplicationInterface();
	if (ReplicationInterface == nullptr) // No available Replication Interface, we are going to drop these calls.
	{	// (By design: someone who wants proxy replication should be ok with GC RPCs being dropped when the proxy is null)
		return;
	}

	FGameplayCueParameters Parameters;
	UAbilitySystemGlobals::Get().InitGameplayCueParameters_GESpec(Parameters, Spec);

	static TArray<FGameplayTag, TInlineAllocator<4>> Tags; Tags.Reset();

	for (const FGameplayEffectCue& EffectCue : Spec.Def->GameplayCues)
	{
		for (const FGameplayTag& Tag: EffectCue.GameplayCueTags)
		{
			if (Tag.IsValid()) Tags.Add(Tag);
		}
	}
	if (Tags.Num() == 1)
	{
		ReplicationInterface->NetMulticast_InvokeGameplayCueAddedAndWhileActive_WithParams(Tags[0], PredictionKey, Parameters);
	}
	else if (Tags.Num() > 1)
	{
		ReplicationInterface->NetMulticast_InvokeGameplayCuesAddedAndWhileActive_WithParams(
			FGameplayTagContainer::CreateFromArray(Tags), PredictionKey, Parameters);
	}
	else UE_LOG(LogTemp, Warning, TEXT("No actual gameplay cue tags found in GameplayEffect %s "
							 "(despite it having entries in its gameplay cue list!"), *Spec.Def->GetName())
}


void UAuraCueManager::AddPendingCueNextTick(FGameplayCuePendingExecute& PendingCue)
{	// AddPendingCueExecuteInternal(PendingCue);
	if (ProcessPendingCueExecute(PendingCue)) PendingExecuteCues.Add(PendingCue);
	if (GameplayCueSendContextCount == 0)
	{	// Not in a context, flush now
		const UWorld* World = PendingCue.OwningComponent->GetWorld();
		if (World == nullptr) return;
		if (PendingHandle.IsValid()) World->GetTimerManager().ClearTimer(PendingHandle);
		PendingHandle = World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([&]()
		{
			PendingHandle.Invalidate();
			FlushPendingCues(); // Flush next tick so that we can batch gameplay cues
		}));
	}
}
