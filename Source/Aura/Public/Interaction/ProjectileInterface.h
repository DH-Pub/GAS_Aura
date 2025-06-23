// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectileInterface.generated.h"

class UNiagaraSystem;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UProjectileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IProjectileInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetImpactCue(UNiagaraSystem*& CueEffect, USoundBase*& CueSound);
};
