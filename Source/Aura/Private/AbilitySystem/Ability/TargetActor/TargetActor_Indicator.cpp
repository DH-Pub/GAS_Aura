// Copyright Hung


#include "AbilitySystem/Ability/TargetActor/TargetActor_Indicator.h"

#include "Components/DecalComponent.h"

ATargetActor_Indicator::ATargetActor_Indicator()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root")); // Root for hierarchyS

	Decal = CreateDefaultSubobject<UDecalComponent>("DecalComponent");
	Decal->SetupAttachment(GetRootComponent());
}

void ATargetActor_Indicator::BeginPlay()
{
	Super::BeginPlay();

}
