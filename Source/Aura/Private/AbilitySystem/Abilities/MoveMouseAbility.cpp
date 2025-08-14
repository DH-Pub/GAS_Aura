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

void UMoveMouseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AuraPlayerController->MouseMovementState = HoldMove;
	// if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)){}
	/*GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Orange, FString::Printf(TEXT("%s"),
	*AuraCharacterFromActorInfo->GetAuraAbilitySystemComponent()->GetFullName()));*/
}

void UMoveMouseAbility::HoldReleased()
{
	Super::HoldReleased();
	AuraPlayerController->MouseMovementState = Stop;
}

void UMoveMouseAbility::TapReleased()
{
	Super::TapReleased();
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

void UMoveMouseAbility::DoubleClick()
{
	Super::DoubleClick();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString("DoubleClick"));
}

void UMoveMouseAbility::TripleClick()
{
	Super::TripleClick();
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString("TripleClick"));
}
