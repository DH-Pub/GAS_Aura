// Copyright Hung


#include "AuraCueManager.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AuraEffectTypes.h"

void UAuraCueManager::FlushPendingCues()
{	// Super::FlushPendingCues();
	if (PendingExecuteCues.Num() == 0) return;
	OnFlushPendingCues.Broadcast();

	FGameplayCuePendingExecute PendingCue = PendingExecuteCues[0];
	UAbilitySystemComponent* ASC = PendingCue.OwningComponent;
	IAbilitySystemReplicationProxyInterface* RepInterface = ASC ? ASC->GetReplicationInterface() : nullptr;
	for (int32 i = 1; i < PendingExecuteCues.Num(); i++)
	{	// Batches the rest of the PendingExecuteCues into the first's EffectContext
		FGameplayCuePendingExecute& ExecuteCue = PendingExecuteCues[i];
		if (ASC == nullptr) ASC = ExecuteCue.OwningComponent; // Our component may have gone away
		if (RepInterface == nullptr)
		{	// If null, "we are replicating through a proxy and have no avatar"
			if (ExecuteCue.OwningComponent) RepInterface = ExecuteCue.OwningComponent->GetReplicationInterface();
			else if (ASC) RepInterface = ASC->GetReplicationInterface();
		}
		FAuraEffectContext* FirstCueContext = FAuraEffectContext::ExtractAuraContext(
			PendingCue.CueParameters.EffectContext);
		/*FGameplayCueParameters& CueParams = FirstCueContext->AddToCuesBatch(ExecuteCue.CueParameters);
		CueParams.AggregatedSourceTags.AddTagFast(ExecuteCue.GameplayCueTags[0]);*/
		FirstCueContext->AddToCoreCuesBatch(ExecuteCue.GameplayCueTags[0], ExecuteCue.CueParameters);
	}
	PendingExecuteCues.Empty();

	if (ASC == nullptr || RepInterface == nullptr) return;
	const bool bHasAuthority = ASC->IsOwnerActorAuthoritative();
	const bool bLocalPredictionKey = PendingCue.PredictionKey.IsLocalClientKey();

	// TODO: Could implement non-rpc method for replicating if desired
	if (!ensure(PendingCue.GameplayCueTags.Num() >= 1)) return;
	if (bHasAuthority)
	{
		RepInterface->ForceReplication();
		if (PendingCue.GameplayCueTags.Num() > 1)
		{
			RepInterface->Call_InvokeGameplayCuesExecuted_WithParams(FGameplayTagContainer::
				CreateFromArray(PendingCue.GameplayCueTags), PendingCue.PredictionKey, PendingCue.CueParameters);
		}
		else
		{
			RepInterface->Call_InvokeGameplayCueExecuted_WithParams(PendingCue.GameplayCueTags[0],
				PendingCue.PredictionKey, PendingCue.CueParameters);
			CheckForTooManyRPCs(TEXT("NetMulticast_InvokeGameplayCueExecuted_WithParams"),
				PendingCue, PendingCue.GameplayCueTags[0].ToString(), nullptr);
		}
	}
	else if (bLocalPredictionKey)
	{
		for (const FGameplayTag& Tag : PendingCue.GameplayCueTags)
		{
			ASC->InvokeGameplayCueEvent(Tag, EGameplayCueEvent::Executed, PendingCue.CueParameters);
		}
	}
}


template<class AllocatorType>
void PullGameplayCueTagsFromSpec(const FGameplayEffectSpec& Spec, TArray<FGameplayTag, AllocatorType>& OutArray)
{
	// Add all GameplayCue Tags from the GE into the GameplayCueTags PendingCue.list
	for (const FGameplayEffectCue& EffectCue : Spec.Def->GameplayCues)
	{
		for (const FGameplayTag& Tag: EffectCue.GameplayCueTags)
		{
			if (Tag.IsValid())
			{
				OutArray.Add(Tag);
			}
		}
	}
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
	PullGameplayCueTagsFromSpec(Spec, PendingCue.GameplayCueTags);
	if (PendingCue.GameplayCueTags.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GE %s has GameplayCues but no valid tag."), *Spec.Def->GetName());			
		return;
	}

	// UAbilitySystemGlobals::Get().InitGameplayCueParameters_GESpec(PendingCue.CueParameters, Spec);
	FAuraEffectContext* AuraContext = FAuraEffectContext::ExtractAuraContext(ContextHandle);
	for (const FGameplayEffectCue& CueDef : Spec.Def->GameplayCues)
	{
		if (!CueDef.MagnitudeAttribute.IsValid() || CueDef.GameplayCueTags.Num() == 0) continue;
		for (const FGameplayEffectModifiedAttribute& ModifiedAttr : Spec.ModifiedAttributes)
		{	// Loop through UGameplayEffect's Modifiers(CalculateBaseMagnitude) and/or Executions(OutExecutionOutput)
			if (ModifiedAttr.Attribute != CueDef.MagnitudeAttribute) continue;
			// PendingCue.CueParameters.RawMagnitude = ModifiedAttr.TotalMagnitude;
			// CueParameters.RawMagnitude won't be set, we will use FCoreEffectCues
			AuraContext->AddToCoreEffectCues(ModifiedAttr.TotalMagnitude, CueDef.GameplayCueTags.GetByIndex(0));
			break;
		}
	}
	if (ContextHandle.IsValid()) PendingCue.CueParameters.EffectContext = ContextHandle;


	// AddPendingCueExecuteInternal(PendingCue) // Finish by Adding to Pending
	if (ProcessPendingCueExecute(PendingCue)) PendingExecuteCues.Add(PendingCue);
	if (GameplayCueSendContextCount == 0)
	{	// Not in a context, flush now
		if (const UWorld* World = OwningComponent->GetWorld())
		{
			World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(OwningComponent, [&]()
			{	// Flush next tick so that we can batch gameplay cue
				FlushPendingCues();
			}));
		}
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

	static TArray<FGameplayTag, TInlineAllocator<4> > Tags; Tags.Reset();

	PullGameplayCueTagsFromSpec(Spec, Tags);
	if (Tags.Num() == 1)
	{
		ReplicationInterface->Call_InvokeGameplayCueAddedAndWhileActive_WithParams(Tags[0], PredictionKey, Parameters);
	}
	else if (Tags.Num() > 1)
	{
		ReplicationInterface->Call_InvokeGameplayCuesAddedAndWhileActive_WithParams(
			FGameplayTagContainer::CreateFromArray(Tags), PredictionKey, Parameters);
	}
	else UE_LOG(LogTemp, Warning, TEXT("No actual gameplay cue tags found in GameplayEffect %s "
							 "(despite it having entries in its gameplay cue list!"), *Spec.Def->GetName())
}
