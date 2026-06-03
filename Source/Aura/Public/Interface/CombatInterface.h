// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UENUM(BlueprintType)
enum class ECombatSocket : uint8
{
	None,
	Weapon,
	LeftHand, RightHand,
	Tail
};

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatSocket SocketEnum = ECombatSocket::Weapon;
};

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * IMPORTANT: BlueprintNativeEvent: [I]Interface::Execute_ is required
 * Use UObject->Implements<[U]Interface>() to detect interface because if interface is implement directly on a Blueprint
 * C++ Cast<Interface> will always return nullptr
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent)
	class UNiagaraSystem* GetBloodEffect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;
};
