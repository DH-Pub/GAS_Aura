// Copyright Hung


#include "AbilitySystem/Abilities/MoveMouseAbility.h"

#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Character/AuraCharacterBase.h"
#include "Components/SplineComponent.h"
#include "Player/AuraPlayerController.h"

UMoveMouseAbility::UMoveMouseAbility()
{
	SetAssetTags(FGameplayTagContainer(AuraGameplayTags::Ability_Move_Mouse));
	StartupInputTag = AuraGameplayTags::Input_Move_Mouse;
	bRetriggerInstancedAbility = true;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UMoveMouseAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	AuraPlayerController->MouseMovementState = Stop;
}

void UMoveMouseAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void UMoveMouseAbility::StartPressedOngoing_Implementation()
{
	Super::StartPressedOngoing_Implementation();
	if (AuraPlayerController == nullptr) return;
	AuraPlayerController->MouseMovementState = HoldMove;
}


void UMoveMouseAbility::TapReleased_Implementation()
{
	Super::TapReleased_Implementation();
	if (AuraPlayerController->GetCursorHitResult().bBlockingHit)
	{
		AuraPlayerController->AutoMoveDestination = AuraPlayerController->GetCursorHitResult().ImpactPoint;
		FNavLocation ImpactPointNavLocation;
		if (NavSystem->ProjectPointToNavigation(AuraPlayerController->AutoMoveDestination, ImpactPointNavLocation, NavExtent,
												&AuraPlayerController->GetNavAgentPropertiesRef()))
		{
			UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this,
				AuraCharacterFromActorInfo->GetActorLocation(), AuraPlayerController->AutoMoveDestination);
			if (NavPath && !NavPath->PathPoints.IsEmpty())
			{
				AuraPlayerController->Spline->ClearSplinePoints();
				for (int32 i = 0; i < NavPath->PathPoints.Num(); i++)
				{
					FVector Position = AuraPlayerController->Spline->GetComponentTransform().InverseTransformPosition(NavPath->PathPoints[i]);
					FSplinePoint SplinePoint(i, Position, ESplinePointType::Linear);
					AuraPlayerController->Spline->AddPoint(SplinePoint);
					if (bDrawNavBox) DrawDebugSphere(GetWorld(), NavPath->PathPoints[i], 25.f, 6, FColor::Yellow, false, 1.f);
				}
				// for (const FVector& PointLoc : NavPath->PathPoints)
				// { Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World); }
				AuraPlayerController->AutoMoveDestination = NavPath->PathPoints.Last();
				AuraPlayerController->MouseMovementState = AutoMove;
				return;
			}
		}
	}
	AuraPlayerController->MouseMovementState = Stop;
}
void UMoveMouseAbility::HoldReleased_Implementation()
{
	Super::HoldReleased_Implementation();
	if (AuraPlayerController == nullptr) return;
	AuraPlayerController->MouseMovementState = Stop;
}


void UMoveMouseAbility::DoubleClick_Implementation()
{
	Super::DoubleClick_Implementation();;
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString("DoubleClick"));
}
void UMoveMouseAbility::TripleClick_Implementation()
{
	Super::TripleClick_Implementation();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString("TripleClick"));
}
