// Copyright Hung


#include "AuraEffectTypes.h"

#include "StructUtils/InstancedStruct.h"

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
		if (bReplicateEffectCauser && EffectCauser.IsValid() )
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

		// Custom -----------------------------------------------------------------------------------------------
		if (bShowDamageOnTarget) RepBits |= 1 << 7;
		if (InstancedStruct.IsValid()) RepBits |= 1 << 8;
		// if (CueLocations.Num() > 0) RepBits |= 1 << 10;
		// End -----------------------------------------------------------------------------------------------------
	}

	// REQUIRED to be modified
	Ar.SerializeBits(&RepBits, 10); // 0-9

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

	// Custom ==========================================================================================
	bShowDamageOnTarget = RepBits & (1 << 7);
	if (RepBits & (1 << 8))
	{
		if (Ar.IsLoading() && !InstancedStruct.IsValid())
		{	// HitResult has been serialized
			InstancedStruct = TSharedPtr<FInstancedStruct>(new FInstancedStruct()); // Set new if not exist
		}
		InstancedStruct->NetSerialize(Ar, Map, bOutSuccess);
	}
	/*if (RepBits & (1<<14))
	{
		// Ar << DeathImpulse;
		// DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);
	}*/
	/*if (RepBits & (1 << 9))
	{
		SafeNetSerializeTArray_WithNetSerialize<31>(Ar, CueLocations, Map);
		// Or modify bOutSuccess
		/#1#/ Must add NetCore in Build.cs
		bOutSuccess &= SafeNetSerializeTArray_WithNetSerialize<31>(Ar, CueLocations, Map);#1#
	}*/
	// End ====================================================================================================

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}
	
	bOutSuccess = true;
	return true;
}
