// Copyright Hung


#include "Actor/AuraBeam.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h" // SystemTemplate = ->GetAsset()

static const FName BeamEndName = FName("BeamEnd");

AAuraBeam::AAuraBeam()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetNetUpdateFrequency(5);

	BeamComponent = CreateDefaultSubobject<UNiagaraComponent>("BeamComponent");
	SetRootComponent(BeamComponent);

	ChainSpawnParams.LocationType = EAttachLocation::KeepRelativeOffset;
	// ChainSpawnParams.bAutoDestroy = true;
}

void AAuraBeam::Destroyed()
{
	for (UNiagaraComponent* Comp : ChainBeamsNiagara)
	{
		if (Comp) Comp->DestroyComponent();
	}
	ChainBeamsNiagara.Reset();

	Super::Destroyed();
}

void AAuraBeam::MulticastSetTarget_Implementation(FGATargetData_BeamData Data)
{
	BeamData = MoveTemp(Data);
	SetBeamByData();
}

void AAuraBeam::BeginPlay()
{
	Super::BeginPlay();

	BeamComponent->SetVariableVec3(BeamEndName, GetActorLocation());
}

void AAuraBeam::SetBeamByData()
{
	if (!BeamComponent->IsActive()) BeamComponent->Activate();

	int32 i = 0;
	ChainSpawnParams.WorldContextObject = this;
	if (BeamData.TargetActorArray.Num() > 0 && BeamData.TargetActorArray[0].IsValid())
	{
		BeamComponent->SetVariableVec3(BeamEndName, BeamData.TargetActorArray[0]->GetActorLocation());
		for (; i < BeamData.TargetActorArray.Num() - 1; i++)
		{
			const AActor* FromTarget = BeamData.TargetActorArray[i].Get();
			const AActor* ToTarget = BeamData.TargetActorArray[i+1].Get();
			if (!FromTarget || !ToTarget)
			{
				break;
			}
			ChainSpawnParams.AttachToComponent = FromTarget->GetRootComponent();
			if (ChainBeamsNiagara.Num() > i)
			{
				if (UNiagaraComponent* Comp = ChainBeamsNiagara[i])
				{	// if (!Comp->IsActive()) Comp->Activate();
					if (!Comp->IsAttachedTo(ChainSpawnParams.AttachToComponent))
					{
						Comp->AttachToComponent(ChainSpawnParams.AttachToComponent,
							FAttachmentTransformRules::KeepRelativeTransform, ChainSpawnParams.AttachPointName);
					}
				}
				else ChainBeamsNiagara[i] = UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(ChainSpawnParams);
			} // Create new component if there's not enough
			else ChainBeamsNiagara.Add(UNiagaraFunctionLibrary::SpawnSystemAttachedWithParams(ChainSpawnParams));

			// ChainBeamsNiagara[i]->SetRelativeLocation(FVector());
			if (ChainBeamsNiagara[i]) ChainBeamsNiagara[i]->SetVariableVec3(BeamEndName, ToTarget->GetActorLocation());
		}
	}
	else if (BeamData.SourceLocation.Num() > 0)
	{
		BeamComponent->SetVariableVec3(BeamEndName, BeamData.SourceLocation[0]);
	}

	for (int32 j = ChainBeamsNiagara.Num() - 1; j >= i; --j) // Iterate backwards so we can remove during loop
	{
		if (ChainBeamsNiagara[j]) ChainBeamsNiagara[j]->DestroyComponent();
		ChainBeamsNiagara.RemoveAt(j); // .Pop()
	}

}

void AAuraBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
