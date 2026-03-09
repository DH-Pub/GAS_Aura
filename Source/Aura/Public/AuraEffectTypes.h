// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayCue_Types.h"
#include "GameplayEffectTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "AuraEffectTypes.generated.h"

/*
 * override AllocGameplayEffectContext in UAuraAbilitySystemGlobals
 */
USTRUCT(BlueprintType)
struct FAuraEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	static FAuraEffectContext* ExtractAuraContext(FGameplayEffectContextHandle ContextHandle)
	{return static_cast<FAuraEffectContext*>(ContextHandle.Get());}

	FInstancedStruct* GetInstancedStruct() const {return InstancedStruct.Get();}
	void SetInstancedStruct(const FInstancedStruct& InStruct) {InstancedStruct = MakeShared<FInstancedStruct>(InStruct);}
	template<typename T>
	const T* GetStruct() const {return InstancedStruct ? InstancedStruct->GetPtr<T>() : nullptr;}
	template<typename T>
	T* GetMutableStruct() {return InstancedStruct ? InstancedStruct->GetMutablePtr<T>() : nullptr;}
#pragma region InstancedStructTemplate
	template<typename T>
	static const T* GetContextStruct(const FGameplayEffectContextHandle& EffectContext)
	{
		const FAuraEffectContext* AuraEffectContext =  static_cast<const FAuraEffectContext*>(EffectContext.Get());
		if (const FInstancedStruct* InstancedStruct = AuraEffectContext->GetInstancedStruct())
		{return InstancedStruct->GetPtr<T>();}
		return nullptr;
	}

	template<typename T>
	static T* MakeStructInContext(FGameplayEffectContextHandle ContextHandle)
	{
		FAuraEffectContext* AuraEffectContext =  static_cast<FAuraEffectContext*>(ContextHandle.Get());
		AuraEffectContext->SetInstancedStruct(FInstancedStruct::Make<T>());
		return AuraEffectContext->GetInstancedStruct()->GetMutablePtr<T>();
	}
	template<typename T>
	static T* GetOrMakeContextStructPtr(FGameplayEffectContextHandle ContextHandle)
	{
		FAuraEffectContext* AuraEffectContext =  static_cast<FAuraEffectContext*>(ContextHandle.Get());
		if (FInstancedStruct* InstancedStruct = AuraEffectContext->GetInstancedStruct())
		{
			return InstancedStruct->GetMutablePtr<T>();
		}
		AuraEffectContext->SetInstancedStruct(FInstancedStruct::Make<T>());
		return AuraEffectContext->GetInstancedStruct()->GetMutablePtr<T>();
	}
#pragma endregion

protected:
	// This will show a drop-down in the editor, containing only MyStruct and its children structs
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/MyModule.MyStruct"))*/
	TSharedPtr<FInstancedStruct> InstancedStruct; // TSharedPtr cannot be UPROPERTY


public: //REQUIRED: override section
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override {return StaticStruct();}

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FAuraEffectContext* Duplicate() const override
	{
		FAuraEffectContext* NewContext = new FAuraEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{	// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}

		// Custom Start ~ =========================================================
		if (GetInstancedStruct())
		{
			NewContext->SetInstancedStruct(*GetInstancedStruct());
		}
		// ~ Custom End ==========================================================
		return NewContext;
	}
	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
};

template<>
struct TStructOpsTypeTraits<FAuraEffectContext> : public TStructOpsTypeTraitsBase2<FAuraEffectContext>
{	// REQUIRED
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
