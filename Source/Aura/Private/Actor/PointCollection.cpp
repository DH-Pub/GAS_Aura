// Copyright Hung


#include "Actor/PointCollection.h"

#include "AuraAbilityLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(Pt_0 = ImmutablePoints.Add_GetRef(CreateDefaultSubobject<USceneComponent>("Pt_0 Component")));

#define CREATE_SUB_POINT(Num)\
	Num = ImmutablePoints.Add_GetRef(CreateDefaultSubobject<USceneComponent>(#Num" Component"));\
	Num->SetupAttachment(Pt_0);
	CREATE_SUB_POINT(Pt_1);
	CREATE_SUB_POINT(Pt_2);
	CREATE_SUB_POINT(Pt_3);
	CREATE_SUB_POINT(Pt_4);
	CREATE_SUB_POINT(Pt_5);
	CREATE_SUB_POINT(Pt_6);
	CREATE_SUB_POINT(Pt_7);
	CREATE_SUB_POINT(Pt_8);
	CREATE_SUB_POINT(Pt_9);
	CREATE_SUB_POINT(Pt_10);
#undef CREATE_SUB_POINT


}

void APointCollection::GetPointsOn(TArray<FHitResult>& OutHits, int32 NumPoints, FCollisionProfileName Profiles,
	float YawOverride, const float Range)
{
	ensureMsgf(ImmutablePoints.Num() >= NumPoints, TEXT("Attempted to access ImmutablePoints out of bounds."));

	if (OutHits.Num() > 0) OutHits.Reset();
	TArray<FHitResult> Results;
	for (USceneComponent* Pt : ImmutablePoints)
	{
		if (OutHits.Num() >= NumPoints) break;
		if (Pt != Pt_0)
		{
			FVector ToPoint = Pt->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Pt->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		FVector PtLoc = Pt->GetComponentLocation();
		const FVector Start = PtLoc + FVector::UpVector * Range;
		const FVector End = PtLoc + FVector::DownVector * Range;
		Results.Reset();
		UAuraAbilityLibrary::TraceByProfile(this, Results, Start, End, {}, Profiles);

		for (FHitResult& Hit : Results)
		{
			if (Hit.bBlockingHit)
			{
				OutHits.Add(MoveTemp(Hit));
				break;
			}
		}
	}
}
