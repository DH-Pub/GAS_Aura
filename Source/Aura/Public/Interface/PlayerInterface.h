// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

// UE_DEPRECATED(5.4, "No use for this")
class AURA_API IPlayerInterface
{
	GENERATED_BODY()
public:
};
