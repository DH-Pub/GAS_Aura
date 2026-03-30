// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor_GroundTrace.h"
#include "TargetActor_Indicator.generated.h"

UCLASS()
class AURA_API ATargetActor_Indicator : public AGameplayAbilityTargetActor_GroundTrace
{
	GENERATED_BODY()

public:
	ATargetActor_Indicator();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UDecalComponent> Decal;
};
