// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "AuraGameplayEffectTypes.generated.h"

struct FInstancedStruct;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	static FAuraGameplayEffectContext* GetAuraContext(FGameplayEffectContextHandle& EffectContextHandle)
	{return static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get());}
	static FAuraGameplayEffectContext* GetAuraContext(FGameplayEffectContext* EffectContext)
	{return static_cast<FAuraGameplayEffectContext*>(EffectContext);}
	
	bool IsBlocked() const {return bIsBlocked;}
	bool SetIsBlocked(const bool bIn) {return bIsBlocked = bIn;}

	bool IsCrit() const {return bIsCrit;}
	bool SetIsCrit(const bool bIn) {return bIsCrit = bIn;}

	bool IsStagger() const {return bStagger;}
	bool SetIsStagger(const bool bIn) {return bStagger = bIn;}

	FInstancedStruct* GetInstancedStruct() const {return InstancedStruct.Get();}
	void SetInstancedStruct(const FInstancedStruct& InStruct);

	UPROPERTY()
	bool bShowDamageOnTarget = false;
	/*UPROPERTY()
	TArray<FVector_NetQuantize> CueLocations = TArray<FVector_NetQuantize>(); // DEPRECATED, using InstancedStruct instead*/

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
protected:
	UPROPERTY()
	bool bIsBlocked = false;
	UPROPERTY()
	bool bIsCrit = false;
	UPROPERTY()
	bool bStagger = false;
	
	// This will show a drop-down in the editor, containing only MyStruct and its children structs
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/MyModule.MyStruct"))
	FInstancedStruct MyInstancedStruct;*/
	TSharedPtr<FInstancedStruct> InstancedStruct; // TSharedPtr cannot be UPROPERTY
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};