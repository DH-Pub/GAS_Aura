// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "AuraEffectTypes.generated.h"


/*
 * Custom GameplayCueParameter with only core elements that you might use
 */
USTRUCT()
struct FCoreGameplayCue
{
	GENERATED_BODY()
	FCoreGameplayCue(){};
	explicit FCoreGameplayCue(const FGameplayTag& Tag, const FGameplayCueParameters& Params);

	void UnpackAndInvokeGameplayCueEvent(UAbilitySystemComponent* ASC) const;

	FGameplayTag CueTag;
	float RawMagnitude = 0.f;
	FGameplayEffectContextHandle EffectContext;
	FVector_NetQuantize10 Location = FVector_NetQuantize10();
	FVector_NetQuantizeNormal Normal = FVector_NetQuantizeNormal();
	UPROPERTY()
	TWeakObjectPtr<AActor> Instigator; // Actor that owns the ability system component
	UPROPERTY()
	TWeakObjectPtr<AActor> EffectCauser; // Can be weapon/projectile

	/** Optimized serializer */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

USTRUCT()
struct FCoreEffectCues
{
	GENERATED_BODY()
	FCoreEffectCues(){}
	explicit FCoreEffectCues(const float RawMagnitude, const FGameplayTag& CueTag)
		: RawMagnitude(RawMagnitude), CueTag(CueTag) {}

	float RawMagnitude = 0.f;
	FGameplayTag CueTag;

	/** Optimized serializer */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		// SafeNetSerializeTArray_Default<31>(Ar, RawMagnitudes);
		Ar << RawMagnitude;
		CueTag.NetSerialize(Ar, Map, bOutSuccess);
		return bOutSuccess = true;
	}
};



/*
 * override AllocGameplayEffectContext from UAuraAbilitySystemGlobals
 */
USTRUCT(BlueprintType)
struct FAuraEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	static FAuraEffectContext* ExtractAuraContext(FGameplayEffectContextHandle ContextHandle)
	{return static_cast<FAuraEffectContext*>(ContextHandle.Get());}

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
		AuraEffectContext->SetInstancedStruct(FInstancedStruct::Make(T()));
		return AuraEffectContext->GetInstancedStruct()->GetMutablePtr<T>();
	}
#pragma endregion


	TArray<FCoreGameplayCue>& GetCoreCuesBatch() {return CoreCuesBatch;}
	FCoreGameplayCue& AddToCoreCuesBatch(const FGameplayTag& Tag, const FGameplayCueParameters& Cue, const bool bReset = false)
	{
		if (bReset && CoreCuesBatch.Num()) CoreCuesBatch.Reset();
		return CoreCuesBatch[CoreCuesBatch.Add(FCoreGameplayCue(Tag, Cue))];
	}

	TArray<FCoreEffectCues>& GetCoreEffectCues() {return CoreEffectCuesList;}
	FCoreEffectCues& AddToCoreEffectCues(const float Magnitude, const FGameplayTag& Cue, const bool bReset = false)
	{
		if (bReset && CoreEffectCuesList.Num()) CoreEffectCuesList.Reset();
		return CoreEffectCuesList[CoreEffectCuesList.Add(FCoreEffectCues(Magnitude, Cue))];
	}
protected:
	UPROPERTY()
	bool bShowDamageOnTarget = false;

	// This will show a drop-down in the editor, containing only MyStruct and its children structs
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/MyModule.MyStruct"))*/
	TSharedPtr<FInstancedStruct> InstancedStruct; // TSharedPtr cannot be UPROPERTY

	UPROPERTY()
	TArray<FCoreGameplayCue> CoreCuesBatch;
	UPROPERTY()
	TArray<FCoreEffectCues> CoreEffectCuesList; // Batch all FGameplayEffectCue here

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
