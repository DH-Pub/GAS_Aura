// Copyright Hung


#include "AuraEffectTypes.h"

#include "AbilitySystemComponent.h"
#include "StructUtils/InstancedStruct.h"

/*
 * Custom GameplayCueParameter with only core elements that you might use
 */
#pragma region Custom GameplayCueParameters
FCoreCueParams::FCoreCueParams(const FGameplayTag& Tag, const FGameplayCueParameters& Params)
{
	CueTag = Tag;
	RawMagnitude = Params.RawMagnitude;
	EffectContext = Params.EffectContext;
	Location = Params.Location;
	Normal = Params.Normal;
	Instigator = Params.Instigator;
	EffectCauser = Params.EffectCauser;
}
void FCoreCueParams::UnpackAndInvokeGameplayCueEvent(UAbilitySystemComponent* ASC) const
{
	FGameplayCueParameters Params(EffectContext);
	Params.RawMagnitude = RawMagnitude;
	Params.Location = Location;
	Params.Normal = Normal;
	Params.Instigator = Instigator;
	Params.EffectCauser = EffectCauser;
	ASC->InvokeGameplayCueEvent(CueTag, EGameplayCueEvent::Executed, Params);
}
bool FCoreCueParams::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	enum ERepFlag
	{
		REP_RawMagnitude,
		REP_EffectContext,
		REP_Location,
		REP_Normal,
		REP_Instigator,
		REP_EffectCauser,
		REP_CueTag,

		REP_MAX // Put this last as the Num(), not for index
	};

	uint8 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (RawMagnitude != 0.f) RepBits |= (1 << REP_RawMagnitude);
		if (EffectContext.IsValid()) RepBits |= (1 << REP_EffectContext);
		if (Location.IsNearlyZero() == false) RepBits |= (1 << REP_Location);
		if (Normal.IsNearlyZero() == false) RepBits |= (1 << REP_Normal);
		if (Instigator.IsValid()) RepBits |= (1 << REP_Instigator);
		if (EffectCauser.IsValid()) RepBits |= (1 << REP_EffectCauser);
		if (CueTag.IsValid()) RepBits |= (1 << REP_CueTag);
	}

	Ar.SerializeBits(&RepBits, REP_MAX);

	if (RepBits & (1 << REP_RawMagnitude)) Ar << RawMagnitude;
	if (RepBits & (1 << REP_EffectContext)) EffectContext.NetSerialize(Ar, Map, bOutSuccess);
	if (RepBits & (1 << REP_Location)) Location.NetSerialize(Ar, Map, bOutSuccess);
	if (RepBits & (1 << REP_Normal)) Normal.NetSerialize(Ar, Map, bOutSuccess);
	if (RepBits & (1 << REP_Instigator)) Ar << Instigator;
	if (RepBits & (1 << REP_EffectCauser)) Ar << EffectCauser;
	if (RepBits & (1 << REP_CueTag)) CueTag.NetSerialize(Ar, Map, bOutSuccess);

	return bOutSuccess = true;
}
#pragma endregion



bool FAuraEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	// -------------------------------------------------------
	uint32 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}

		//IMPORTANT: Add Custom here -------------------------------------------------------------------------------
		if (bShowDamageOnTarget) RepBits |= 1 << 7;
		if (InstancedStruct.IsValid()) RepBits |= 1 << 8;
		if (CueParamsBatched.Num() > 0) RepBits |= 1 << 9;
		if (EffectCuesList.Num() > 0) RepBits |= 1 << 10;
		// End -----------------------------------------------------------------------------------------------------
	}

	//IMPORTANT: Required to be modified
	Ar.SerializeBits(&RepBits, 11); // 0-10

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}

	//IMPORTANT: Custom ==========================================================================================
	bShowDamageOnTarget = RepBits & (1 << 7);
	if (RepBits & (1 << 8))
	{
		if (Ar.IsLoading() && !InstancedStruct.IsValid())
		{	// HitResult has been serialized
			// InstancedStruct = TSharedPtr<FInstancedStruct>(new FInstancedStruct()); // Set new if not exist
			InstancedStruct = MakeShared<FInstancedStruct>(); // Set new if not exist
		}
		InstancedStruct->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 9))
	{	// Must add NetCore in Build.cs
		SafeNetSerializeTArray_WithNetSerialize<31>(Ar, CueParamsBatched, Map);
	}
	if (RepBits & (1 << 10))
	{	// Must add NetCore in Build.cs
		SafeNetSerializeTArray_WithNetSerialize<31>(Ar, EffectCuesList, Map);
	}
	/*if (RepBits & (1<<14)) // Ar << DeathImpulse; // DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);*/
	/*if (RepBits & (1 << 9))
	{
		SafeNetSerializeTArray_WithNetSerialize<31>(Ar, CueLocations, Map);
		/#1#/ Must add NetCore in Build.cs
		bOutSuccess &= SafeNetSerializeTArray_WithNetSerialize<31>(Ar, CueLocations, Map); // Or modify bOutSuccess #1# 
	}*/
	// End ====================================================================================================

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}

	return bOutSuccess = true;
}
