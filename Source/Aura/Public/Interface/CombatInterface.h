// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class AAuraCharacterBase;
enum class ECharacterClass : uint8;
class UNiagaraSystem;

UENUM(BlueprintType)
enum class ECombatSocket : uint8
{
	Weapon,
	LeftHand,
	RightHand,
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> ImpactSound = nullptr;
};

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AActor*, DeadActor);

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/** TODO: Remember this
 * if BlueprintNativeEvent, [I]Interface::Execute_ is required
 * Use UObject->Implements<[U]Interface>() to detect interface because if interface is implement directly on a Blueprint
 * C++ Cast<Interface> will always return nullptr
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()
public:
	/*UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetCharacterLevel();*/
	UFUNCTION(BlueprintNativeEvent)
	UNiagaraSystem* GetBloodEffect();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;
	/*
	 * GetOnDeathDelegate().AddDynamic(this, &);}
	 * returns reference or ptr or AddDynamic will create a copy
	 */
	// virtual FOnDeath& GetOnDeathDelegate() = 0;
};
