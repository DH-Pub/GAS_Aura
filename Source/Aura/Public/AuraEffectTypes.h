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
struct FCoreCueParams
{
	GENERATED_BODY()
	FCoreCueParams(){};
	explicit FCoreCueParams(const FGameplayTag& Tag, const FGameplayCueParameters& Params);

	void UnpackAndInvokeGameplayCueEvent(UAbilitySystemComponent* ASC) const;

	UPROPERTY()
	FGameplayTag CueTag;
	float RawMagnitude = 0.f;
	UPROPERTY()
	FGameplayEffectContextHandle EffectContext;
	UPROPERTY()
	FVector_NetQuantize10 Location = FVector_NetQuantize10();
	UPROPERTY()
	FVector_NetQuantizeNormal Normal = FVector_NetQuantizeNormal();
	UPROPERTY()
	TWeakObjectPtr<AActor> Instigator; // Actor that owns the ability system component
	UPROPERTY()
	TWeakObjectPtr<AActor> EffectCauser; // Can be weapon/projectile

	bool operator==(const FCoreCueParams& Other) const
	{
		return CueTag == Other.CueTag
		&& RawMagnitude == Other.RawMagnitude
		&& EffectContext == Other.EffectContext
		&& Location == Other.Location
		&& Normal == Other.Normal
		&& Instigator == Other.Instigator
		&& EffectCauser == Other.EffectCauser;
	}

	/** Optimized serializer */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

USTRUCT()
struct FEffectCues
{
	GENERATED_BODY()
	FEffectCues(){}
	explicit FEffectCues(const FGameplayTag& CueTag, const float RawMagnitude)
		: CueTag(CueTag), RawMagnitude(RawMagnitude) {}

	UPROPERTY()
	FGameplayTag CueTag;
	float RawMagnitude = 0.f;

	bool operator==(const FEffectCues& Other) const
	{
		return (CueTag == Other.CueTag) && (RawMagnitude == Other.RawMagnitude);
	}

	/** Optimized serializer */
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		CueTag.NetSerialize(Ar, Map, bOutSuccess);
		Ar << RawMagnitude; // SafeNetSerializeTArray_Default<31>(Ar, RawMagnitudes);
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


	TArray<FCoreCueParams>& GetCueParamsBatched() {return CueParamsBatched;}
	// Add CueParams to CueParamsBatched, check if Cue.EffectContext.Get() is NOT self (this) before adding
	int32 BatchCuesParams(const FGameplayTag& Tag, const FGameplayCueParameters& Cue, const bool bReset = false)
	{
		if (Cue.EffectContext.Get() == this) return 0; // Adding itself will cause error in NetSerialize to Client
		if (bReset && CueParamsBatched.Num()) CueParamsBatched.Reset();
		return CueParamsBatched.AddUnique(FCoreCueParams(Tag, Cue));
	}

	TArray<FEffectCues>& GetEffectCuesList() {return EffectCuesList;}
	// Batch all FGameplayEffectCue in effect
	FEffectCues& AddToEffectCuesList(const FGameplayTag& Cue, const float Magnitude, const bool bReset = false)
	{
		if (bReset && EffectCuesList.Num()) EffectCuesList.Reset();
		return EffectCuesList[EffectCuesList.AddUnique(FEffectCues(Cue, Magnitude))];
	}
protected:
	UPROPERTY()
	bool bShowDamageOnTarget = false;

	// This will show a drop-down in the editor, containing only MyStruct and its children structs
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BaseStruct = "/Script/MyModule.MyStruct"))*/
	TSharedPtr<FInstancedStruct> InstancedStruct; // TSharedPtr cannot be UPROPERTY

	UPROPERTY()
	TArray<FCoreCueParams> CueParamsBatched;
	UPROPERTY()
	TArray<FEffectCues> EffectCuesList; // Batch all FGameplayEffectCue here

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
