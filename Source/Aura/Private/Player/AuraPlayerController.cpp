// Copyright Hung


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Camera/CameraComponent.h"
#include "Character/AuraCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Kismet/KismetMaterialLibrary.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Splines");
}

void AAuraPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	SetCameraCapsule();
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Mouse, false, CursorHitResult);
	if (!CursorHitResult.bBlockingHit) return;

	LastActor = CurrentActor;
	CurrentActor = CursorHitResult.GetActor(); // cast to IEnemyInterface, nullptr if can't (i.e. Floor -> nullptr)

	if (CurrentActor != LastActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (CurrentActor) CurrentActor->HighlightActor();
	}
}
void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			GetPawn()->GetActorLocation(), ESplineCoordinateSpace::World);
		// FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		FVector Direction = Spline->FindTangentClosestToWorldLocation(GetPawn()->GetActorLocation(), ESplineCoordinateSpace::World);
		Direction += LocationOnSpline - GetPawn()->GetActorLocation();
		GetPawn()->AddMovementInput(Direction);
		
		const float DistanceToDestinationSquared = (LocationOnSpline - CachedDestination).SizeSquared();
		if (DistanceToDestinationSquared < AutoRunAcceptanceRadius * AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
			Spline->ClearSplinePoints();
		}
	}
}

void AAuraPlayerController::BeginPlay()
{
	check(AuraContext); // check/verify/ensure
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
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
	
	AuraInputComponent->BindAbilityActions(InputConfig, this,
		&ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}


// ======================================================================================================================================
#pragma region Occlusion
void AAuraPlayerController::SetCameraCapsule()
{
	// APawn::Controller might replicate before AController::Pawn so GetPawn() might be nullptr
	if (!IsLocalController()) return;
	if (AAuraCharacter* AuraCharacter = Cast<AAuraCharacter>(GetPawn()))
	{
		ActiveSpringArm = Cast<USpringArmComponent>(AuraCharacter->GetComponentByClass(USpringArmComponent::StaticClass()));
		ActiveCamera = Cast<UCameraComponent>(AuraCharacter->GetComponentByClass(UCameraComponent::StaticClass()));
		CameraCapsule = Cast<UCapsuleComponent>(AuraCharacter->GetCameraCapsule());
		if (CameraCapsule->OnComponentBeginOverlap.IsBound() || CameraCapsule->OnComponentEndOverlap.IsBound())
		{
			CameraCapsule->OnComponentBeginOverlap.Clear();
			CameraCapsule->OnComponentEndOverlap.Clear();
		}
		CameraCapsule->OnComponentBeginOverlap.AddDynamic(this, &AAuraPlayerController::OnCameraCapsuleOverlap);
		CameraCapsule->OnComponentEndOverlap.AddDynamic(this, &AAuraPlayerController::OnCameraCapsuleEndOverlap);
	}
}

void AAuraPlayerController::OnCameraCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TArray<UStaticMeshComponent*> StaticMeshes;
	OtherActor->GetComponents(UStaticMeshComponent::StaticClass(), StaticMeshes);
	for (UStaticMeshComponent* Mesh : StaticMeshes)
	{
		FCameraOccludedStaticMesh OccludedStaticMesh;
		int32 i = 0;
		for (auto Material : Mesh->GetMaterials())
		{
			UMaterialInstanceDynamic* FadeMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, FadeMaterial);
			OccludedStaticMesh.Materials.Add(Material, FadeMID);
			Mesh->SetMaterial(i++, FadeMID);
			FadeMID->SetScalarParameterValue(FName("Fade"), FadeIntensity);
		}
		OccludedMeshes.Add(Mesh, OccludedStaticMesh);
	}
}
void AAuraPlayerController::OnCameraCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TArray<UStaticMeshComponent*> StaticMeshes;
	OtherActor->GetComponents(UStaticMeshComponent::StaticClass(), StaticMeshes);
	for (UStaticMeshComponent* Mesh : StaticMeshes)
	{
		int32 i = 0;
		for (TPair Material : OccludedMeshes.FindRef(Mesh).Materials)
		{
			// Material.Value->SetScalarParameterValue(FName("Fade"), 1.f);
			Mesh->SetMaterial(i++, Material.Key);
		}
		OccludedMeshes.Remove(Mesh);
	}
}
#pragma endregion
// ==================================================================================================================================


void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetPawn() == nullptr) return;
	bAutoRunning = false;
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0., Rotation.Yaw, 0.);

	// Camera to player leveled to the ground
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->GetMovementComponent()->AddInputVector(ForwardDirection * InputAxisVector.Y + RightDirection * InputAxisVector.X);
	/*GetPawn()->AddMovementInput(ForwardDirection, InputAxisVector.Y);
	GetPawn()->AddMovementInput(RightDirection, InputAxisVector.X);*/
}


void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(AuraGameplayTags::Controls_Move))
	{
		bTargeting = CurrentActor ? true : false;
		bAutoRunning = false;
	}
}
void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC()) AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	if (!InputTag.MatchesTagExact(AuraGameplayTags::Controls_Move) || bTargeting)
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
				UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
					this, GetPawn()->GetActorLocation(), CachedDestination);
				if (NavPath && !NavPath->PathPoints.IsEmpty())
				{
					Spline->ClearSplinePoints();
					for (int32 i = 0; i < NavPath->PathPoints.Num(); i++)
					{
						FVector Position = Spline->GetComponentTransform().InverseTransformPosition(NavPath->PathPoints[i]);
						FSplinePoint SplinePoint(i, Position, ESplinePointType::Linear);
						Spline->AddPoint(SplinePoint);
						if (bDrawNavBox) DrawDebugSphere(GetWorld(), NavPath->PathPoints[i], 25.f, 6, FColor::Yellow, false, 1.f);
					}
					// for (const FVector& PointLoc : NavPath->PathPoints)
					// { Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World); }
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
void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(AuraGameplayTags::Controls_Move) || bTargeting || bShiftKeyDown)
	{
		// Call AbilitySystemComponent
		if (GetASC())
		{
			AbilitySystemComponent->AbilityInputTagHeld(InputTag);
		}
	}
	else if (!bTargeting)
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHitResult.bBlockingHit)
		{
			CachedDestination = CursorHitResult.ImpactPoint;
		}

		if (GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - GetPawn()->GetActorLocation()).GetSafeNormal();
			GetPawn()->AddMovementInput(WorldDirection);
		}
	}
}


UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AbilitySystemComponent;
}
