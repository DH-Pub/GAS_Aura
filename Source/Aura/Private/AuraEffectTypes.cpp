// Copyright Hung


#include "AuraEffectTypes.h"

#include "AbilitySystemComponent.h"
#include "StructUtils/InstancedStruct.h"

bool FAuraEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{	//Super::NetSerialize(Ar, Map, bOutSuccess);
	// -------------------------------------------------------
	enum ERepFlag
	{
		REP_Instigator,
		REP_EffectCauser,
		REP_AbilityCDO,
		REP_SourceObject,
		REP_Actors,
		REP_HitResult,
		REP_WorldOrigin,

		REP_InstancedStruct,

		REP_MAX // Put this last as the Num(), not for index
	};

	uint8 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid()) RepBits |= 1 << REP_Instigator;
		if (bReplicateEffectCauser && EffectCauser.IsValid()) RepBits |= 1 << REP_EffectCauser;
		if (AbilityCDO.IsValid()) RepBits |= 1 << REP_AbilityCDO;
		if (bReplicateSourceObject && SourceObject.IsValid()) RepBits |= 1 << REP_SourceObject;
		if (Actors.Num() > 0) RepBits |= 1 << REP_Actors;
		if (HitResult.IsValid()) RepBits |= 1 << REP_HitResult;
		if (bHasWorldOrigin) RepBits |= 1 << REP_WorldOrigin;

		//IMPORTANT: Add Custom here -------------------------------------------------------------------------------
		if (InstancedStruct.IsValid()) RepBits |= 1 << REP_InstancedStruct;
		// End -----------------------------------------------------------------------------------------------------
	}

	Ar.SerializeBits(&RepBits, REP_MAX); // LengthBits is Num()

	if (RepBits & (1 << REP_Instigator)) Ar << Instigator;
	if (RepBits & (1 << REP_EffectCauser)) Ar << EffectCauser;
	if (RepBits & (1 << REP_AbilityCDO)) Ar << AbilityCDO;
	if (RepBits & (1 << REP_SourceObject)) Ar << SourceObject;
	if (RepBits & (1 << REP_Actors)) SafeNetSerializeTArray_Default<31>(Ar, Actors);
	if (RepBits & (1 << REP_HitResult))
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
	if (RepBits & (1 << REP_WorldOrigin))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}

	//IMPORTANT: Custom ==========================================================================================
	if (RepBits & (1 << REP_InstancedStruct))
	{
		if (Ar.IsLoading() && !InstancedStruct.IsValid())
		{	// InstancedStruct = TSharedPtr<FInstancedStruct>(new FInstancedStruct()); // Set new if not exist
			InstancedStruct = MakeShared<FInstancedStruct>(); // Make new if not exist
		}
		InstancedStruct->NetSerialize(Ar, Map, bOutSuccess);
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
