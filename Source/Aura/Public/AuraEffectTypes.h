// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "AuraEffectTypes.generated.h"

struct FInstancedStruct;

/*
 * return from UAuraAbilitySystemGlobals
 */
USTRUCT(BlueprintType)
struct FAuraEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	static FAuraEffectContext* ExtractAuraContext(FGameplayEffectContext* Context)
	{return static_cast<FAuraEffectContext*>(Context);}
	static FAuraEffectContext* ExtractAuraContext(FGameplayEffectContextHandle ContextHandle)
	{return static_cast<FAuraEffectContext*>(ContextHandle.Get());}
	static const FAuraEffectContext* ExtractAuraEffectContext(const FGameplayEffectContextHandle& ContextHandle)
	{return static_cast<const FAuraEffectContext*>(ContextHandle.Get());}

	bool IsShowDamageOnTarget() const {return bShowDamageOnTarget;}
	bool SetShowDamageOnTarget(const bool bIn) {return bShowDamageOnTarget = bIn;}
	static void SetIsShowDamageOnTarget(FGameplayEffectContext* EffectContext, const bool bValue)
	{
		if (FAuraEffectContext* AuraEffectContext =  static_cast<FAuraEffectContext*>(EffectContext))
		{AuraEffectContext->SetShowDamageOnTarget(bValue);}
	}

	FInstancedStruct* GetInstancedStruct() const {return InstancedStruct.Get();}
	void SetInstancedStruct(const FInstancedStruct& InStruct)
	{InstancedStruct = MakeShared<FInstancedStruct>(InStruct);}
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
	// When you are sure this is valid
	template<typename T>
	static T& GetContextMutableStruct(const FGameplayEffectContextHandle& EffectContext)
	{
		const FAuraEffectContext* AuraEffectContext =  static_cast<const FAuraEffectContext*>(EffectContext.Get());
		FInstancedStruct* InstancedStruct = AuraEffectContext->GetInstancedStruct();
		return InstancedStruct->GetMutable<T>();
	}

	template<typename T>
	static void MakeStructAndAddToContext(FGameplayEffectContext* EffectContext, const T& Struct)
	{
		FAuraEffectContext* AuraEffectContext =  static_cast<FAuraEffectContext*>(EffectContext);
		const FInstancedStruct InstancedStruct = FInstancedStruct::Make(Struct);
		if (!InstancedStruct.IsValid()) return;
		AuraEffectContext->SetInstancedStruct(InstancedStruct);
	}
	template<typename T>
	static T& GetOrMakeContextStructRef(FGameplayEffectContext* EffectContext)
	{
		FAuraEffectContext* AuraEffectContext =  static_cast<FAuraEffectContext*>(EffectContext);
		if (FInstancedStruct* InstancedStruct = AuraEffectContext->GetInstancedStruct())
		{
			if (InstancedStruct->GetPtr<T>()) return InstancedStruct->GetMutable<T>();
		}
		AuraEffectContext->SetInstancedStruct(FInstancedStruct::Make(T()));
		return AuraEffectContext->GetInstancedStruct()->GetMutable<T>();
	}
#pragma endregion


	UE_DEPRECATED(all, "Use InstancedStruct instead, make a USTRUCT, that will store the array")
	UPROPERTY()
	TArray<FVector_NetQuantize> CueLocations = TArray<FVector_NetQuantize>();

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
protected:
	UPROPERTY()
	bool bShowDamageOnTarget = false;

	// This will show a drop-down in the editor, containing only MyStruct and its children structs
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/MyModule.MyStruct"))
	FInstancedStruct MyInstancedStruct;*/
	TSharedPtr<FInstancedStruct> InstancedStruct; // TSharedPtr cannot be UPROPERTY
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