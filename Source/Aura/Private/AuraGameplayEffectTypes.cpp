// Copyright Hung


#include "AuraGameplayEffectTypes.h"

#include "StructUtils/InstancedStruct.h"

void FAuraGameplayEffectContext::AddInstancedStruct(const FInstancedStruct& InStruct)
{
	InstancedStruct = TSharedPtr<FInstancedStruct>(new FInstancedStruct(InStruct));
}

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
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
		if (bIsBlocked) RepBits |= 1 << 7;
		if (bIsCrit) RepBits |= 1 << 8;
		if (bStagger)RepBits |= 1 << 9;
		if (bShowDamageOnTarget) RepBits |= 1 << 10;
		if (InstancedStruct.IsValid()) RepBits |= 1 << 11;
		// End -----------------------------------------------------------------------------------------------------
	}

	Ar.SerializeBits(&RepBits, 12); // 0-11

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
	bIsBlocked = RepBits & (1 << 7);
	bIsCrit = RepBits & (1 << 8);
	bStagger = RepBits & (1 << 9);
	bShowDamageOnTarget = RepBits & (1 << 10);
	if (RepBits & (1 << 11))
	{
		if (Ar.IsLoading() && !InstancedStruct.IsValid())
		{
			InstancedStruct = TSharedPtr<FInstancedStruct>(new FInstancedStruct());
		}
		InstancedStruct->NetSerialize(Ar, Map, bOutSuccess);
	}
	// End ====================================================================================================

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}
	
	bOutSuccess = true;
	/*// Must add NetCore in Build.cs
	bOutSuccess &= SafeNetSerializeTArray_WithNetSerialize<31>(Ar, CueLocations, Map);*/
	return true;
}
