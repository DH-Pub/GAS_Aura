// Copyright Hung


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Splines");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHitResult);
	if (!CursorHitResult.bBlockingHit) return;

	LastActor = CurrentActor;
	CurrentActor = CursorHitResult.GetActor(); // cast to enemy interface, nullptr if can't (i.e. Floor -> nullptr)

	if (CurrentActor != LastActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (CurrentActor) CurrentActor->HighlightActor();
	}
}
void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (ControlledPawn)
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).SquaredLength();
		if (DistanceToDestination < AutoRunAcceptanceRadius * AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}


void AAuraPlayerController::BeginPlay()
{
	check(AuraContext); // check/verify/ensure
	Super::BeginPlay();
	ControlledPawn = GetPawn();

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		InputSystem->AddMappingContext(AuraContext, 0);
	}

	// Mouse Cursor Settings
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPress);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	
	AuraInputComponent->BindAbilityActions(
		InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	bAutoRunning = false;
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0., Rotation.Yaw, 0.);

	// Camera to player leveled to the ground
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (ControlledPawn)
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(AuraGameplayTags::Input_RMB))
	{
		bTargeting = CurrentActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC()) AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	if (!InputTag.MatchesTagExact(AuraGameplayTags::Input_RMB) || bTargeting)
	{
		if (GetASC()) AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
	else if (!bTargeting && !bShiftKeyDown)
	{
		if (FollowTime <= ShortPressThreshold)
		{
			FNavLocation ImpactPointNavLocation;
			if (NavSystem->ProjectPointToNavigation(CursorHitResult.ImpactPoint, ImpactPointNavLocation, NavExtent,
			                                        &GetNavAgentPropertiesRef()))
			{
				if (ControlledPawn == nullptr) ControlledPawn = GetPawn();
				UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
					this, ControlledPawn->GetActorLocation(), CachedDestination);
				if (NavPath && NavPath->PathPoints.Num() > 0)
				{
					Spline->ClearSplinePoints();
					for (int i = 0; i < NavPath->PathPoints.Num(); i++)
					{
						FVector Position = Spline->GetComponentTransform().InverseTransformPosition(NavPath->PathPoints[i]);
						FSplinePoint SplinePoint(i, Position, ESplinePointType::Linear);
						Spline->AddPoint(SplinePoint);
						if (bDrawNavBox) DrawDebugSphere(GetWorld(), NavPath->PathPoints[i], 25.f, 6, FColor::Yellow, false, 1.f);
					}
					// for (const FVector& PointLoc : NavPath->PathPoints)
					// {
					// 	Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					// }
					CachedDestination = NavPath->PathPoints.Last();
					bAutoRunning = true;
				}
			}
			if (bDrawNavBox) DrawDebugBox(GetWorld(), CursorHitResult.ImpactPoint, NavExtent, FColor::Silver, false, 2.0f);
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(AuraGameplayTags::Input_RMB) || bTargeting || bShiftKeyDown)
	{
		if (GetASC()) AbilitySystemComponent->AbilityInputTagHeld(InputTag);
	}
	else if (!bTargeting)
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHitResult.bBlockingHit)
		{
			CachedDestination = CursorHitResult.ImpactPoint;
		}

		if (ControlledPawn)
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ControlledPawn));
	}
	return AbilitySystemComponent;
}
