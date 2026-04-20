// Copyright Hung


#include "AbilitySystem/Ability/TargetData//TargetActor_Indicator.h"

#include "AuraAbilityLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Character/AuraCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ATargetActor_Indicator::ATargetActor_Indicator()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root")); // Root for hierarchyS

	Decal = CreateDefaultSubobject<UDecalComponent>("DecalComponent");
	Decal->SetupAttachment(GetRootComponent());
}

void ATargetActor_Indicator::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATargetActor_Indicator, Details);
}

void ATargetActor_Indicator::ServerSetLocation_Implementation(FVector_NetQuantize10 InLoc,
	FVector_NetQuantizeNormal Direction)
{
	// Server double - check
	if (!Direction.IsNormalized()) Direction.Normalize();
	TArray<FHitResult> Results;
	UAuraAbilityLibrary::TraceByProfile(this, Results, InLoc - Direction * 10.f,
		InLoc + Direction * 1'000'00.f, {}, Details.TraceProfile, 0.f, false, Debug);


	for (const FHitResult& Hit : Results)
	{
		if (!Hit.bBlockingHit) continue;
		InLoc = Hit.ImpactPoint;
		break;
	}

	MulticastSetLocation(InLoc, Direction);
}

void ATargetActor_Indicator::MulticastSetLocation_Implementation(const FVector_NetQuantize10 InLoc,
	const FVector_NetQuantizeNormal Direction)
{
	CheckAndSetLocation(InLoc, Direction);
}

void ATargetActor_Indicator::CheckAndSetLocation(FVector InLoc, FVector Direction)
{
	bool bValidLocation = true;
	ensure(Details.MaxRange > 0.f);

	if (!Details.OriginComponent) return;
	const FVector OriginLoc = Details.OriginComponent->GetComponentLocation();
	const FVector VecToLoc = InLoc - OriginLoc;
	const float DistXYSqr = VecToLoc.SizeSquared2D();
	if (DistXYSqr > Details.MaxRange * Details.MaxRange || VecToLoc.Z > Details.MaxRange)
	{
		FVector MaxVec = VecToLoc.GetClampedToMaxSize2D(Details.MaxRange);
		MaxVec.Z = Details.MaxRange; // to trace from the top down
		const FVector Start = OriginLoc + MaxVec;
		const FVector End = Start + FVector::DownVector * (Details.MaxRange + .5f) * 2.f;

		TArray<FHitResult> Results;
		UAuraAbilityLibrary::TraceByProfile(this, Results, Start, End, {},
			Details.TraceProfile, 0.f, false, Debug);
		if (Results.Num() > 0)
		{
			InLoc = Results[0].ImpactPoint;
		}
		else bValidLocation = false;
	}
	if (!bValidLocation)
	{
		if (AAuraCharacterBase* Character = Cast<AAuraCharacterBase>(Details.OriginComponent->GetOwner()))
		{
			const float CapsuleHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			InLoc = OriginLoc + FVector::DownVector * (CapsuleHeight + 5.f);
			//TODO: Show disabled visual (Make this red or something)
		}
	}
	SetActorLocation(InLoc);
}

FGameplayAbilityTargetDataHandle ATargetActor_Indicator::GetIndicatorDataHandle_Implementation()
{
	FGATargetData_CommonTarget* Data = new FGATargetData_CommonTarget();
	Data->Location = GetActorLocation();
	return FGameplayAbilityTargetDataHandle(Data);
}

void ATargetActor_Indicator::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Actors; UGameplayStatics::GetAllActorsOfClass(this, StaticClass(), Actors);
	for (AActor* Actor : Actors)
	{
		ATargetActor_Indicator* Indicator = Cast<ATargetActor_Indicator>(Actor);
		if (Indicator != this && Indicator->Details.OriginComponent == Details.OriginComponent)
		{
			Indicator->OnServerSpawned.Broadcast(this);
		}
	}
}
