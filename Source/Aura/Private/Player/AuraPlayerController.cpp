// Copyright Hung


#include "Player/AuraPlayerController.h"

#include "AuraAbilityLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AuraInputDataAsset.h"
#include "AI/NavigationSystemBase.h"
#include "Animation/AnimInstanceProxy.h"
#include "Aura/Aura.h"
#include "Character/AuraEnemy.h"
#include "Character/AuraPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Input/AuraInputComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Splines");
}

// This is only called for locally controlled PlayerControllers
void AAuraPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (AuraPawn == nullptr) return;
	UpdateAim();
	CursorTick();

	switch (MovementState)
	{
	case Stop: break;
	case AutoMove:
		const FVector PawnLoc = GetPawn()->GetActorLocation();
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(PawnLoc, ESplineCoordinateSpace::World);
		// Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World); // normalized
		FVector Direction = Spline->FindTangentClosestToWorldLocation(PawnLoc, ESplineCoordinateSpace::World);
		Direction += LocationOnSpline - PawnLoc; // Prevent moving off-track
		GetPawn()->AddMovementInput(Direction);
		if ((LocationOnSpline - AuraPawn->AutoMoveDestination).SizeSquared() < AutoRunAcceptanceRadius * AutoRunAcceptanceRadius)
		{
			Spline->ClearSplinePoints();
			MovementState = Stop;
		}
		break;
	case HoldMove:
		GetPawn()->GetMovementComponent()->AddInputVector(AuraPawn->AimDirection);
		break;
	}
}

void AAuraPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	AuraPawn = Cast<AAuraCharacterBase>(InPawn);
	AuraASC = AuraPawn ? AuraPawn->GetAuraAbilitySystemComponent() : nullptr;
	SetCameraComponent();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem
		<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSystem->AddMappingContext(AuraInputDA->InputMappingContext, 0);
	}

	// Mouse Cursor Settings
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}
void AAuraPlayerController::SetupInputComponent()
{	// AuraInputComponent->BindActionValue(InputAction).GetValue();
	Super::SetupInputComponent();

	UAuraInputComponent* InputComp = CastChecked<UAuraInputComponent>(InputComponent);
	InputComp->BindAction(AuraInputDA->MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	InputComp->BindAction(AuraInputDA->MouseInputAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::MoveMouseTriggered);
	InputComp->BindAction(AuraInputDA->MouseInputAction, ETriggerEvent::Completed, this, &AAuraPlayerController::MoveMouseComplete);
}

void AAuraPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (AuraASC) AuraASC->ProcessAbilityInput(DeltaTime, bGamePaused);

	Super::PostProcessInput(DeltaTime, bGamePaused);
}


// ======================================================================================================================================
#pragma region Occlusion
void AAuraPlayerController::SetCameraComponent()
{
	// APawn::Controller might replicate before AController::Pawn so GetPawn() might be nullptr
	if (!IsLocalController()) return;
	if (AAuraPlayer* AuraCharacter = GetPawn<AAuraPlayer>())
	{	/*ActiveSpringArm = Cast<USpringArmComponent>(AuraCharacter->GetComponentByClass(USpringArmComponent::StaticClass()));
		ActiveCamera = Cast<UCameraComponent>(AuraCharacter->GetComponentByClass(UCameraComponent::StaticClass()));*/
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
		for (auto DefaultMaterial : Mesh->GetMaterials())
		{
			UMaterialInstanceDynamic* FadeMID = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, FadeMaterial);
			OccludedStaticMesh.DefaultMaterials.Add(DefaultMaterial);
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
		for (UMaterialInterface* Material : OccludedMeshes.FindRef(Mesh).DefaultMaterials)
		{
			Mesh->SetMaterial(i++, Material);
		}
		OccludedMeshes.Remove(Mesh);
	}
}
#pragma endregion
// ==================================================================================================================================


void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetPawn() == nullptr) return;
	MovementState = Stop;
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	// const FRotator Rotation = GetControlRotation(); // Camera->bUsePawnControlRotation has to be true for this to work
	// GetPlayerViewPoint(); // Called in APawn::GetBaseAimRotation()
	const FRotator Rotation = PlayerCameraManager->GetCameraRotation();
	const FRotator YawRotation(0., Rotation.Yaw, 0.);
	// Camera to player leveled to the ground
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetPawn()->GetMovementComponent()->AddInputVector(ForwardDirection * InputAxisVector.Y + RightDirection * InputAxisVector.X);
}

void AAuraPlayerController::MoveMouseTriggered(const FInputActionValue& InputActionValue)
{
	if (GetPawn() == nullptr) return;
	if (MovementState != HoldMove) MovementState = HoldMove;
	MoveHoldTime += GetWorld()->GetDeltaSeconds();
}
void AAuraPlayerController::MoveMouseComplete(const FInputActionValue& InputActionValue)
{
	MovementState = Stop;
	if (MoveHoldTime > HoldTimeThreshold)
	{
		MoveHoldTime = 0.f;
		if (AuraPawn) AuraPawn->GetCharacterMovement()->StopActiveMovement();
		return;
	}
	MoveHoldTime = 0.f;
	if (!CursorHitResult.bBlockingHit) return;
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys == nullptr) return;
	FNavLocation DestinationNavLocation;
	if (NavSys == nullptr || !NavSys->ProjectPointToNavigation(CursorHitResult.ImpactPoint, DestinationNavLocation,
		NavExtent, &AuraPawn->GetNavAgentPropertiesRef())) return; // Get the closest point to a point location that might be non-reachable within NavExtent
	AuraPawn->AutoMoveDestination = DestinationNavLocation;
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this,
		AuraPawn->GetActorLocation(), AuraPawn->AutoMoveDestination);
	if (NavPath == nullptr || NavPath->PathPoints.IsEmpty()) return;
	MovementState = AutoMove;
	Spline->ClearSplinePoints();
	for (int32 i = 0; i < NavPath->PathPoints.Num(); i++)
	{
		FVector Position = Spline->GetComponentTransform().InverseTransformPosition(NavPath->PathPoints[i]);
		FSplinePoint SplinePoint(i, Position, ESplinePointType::Linear);
		Spline->AddPoint(SplinePoint);
		if (bDrawNavBox) DrawDebugSphere(GetWorld(), NavPath->PathPoints[i], 25.f, 6,
			FColor::Yellow, false, 1.f);
	}
	// for (FVector& PointLoc : NavPath->PathPoints) {Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);}
	AuraPawn->AutoMoveDestination = NavPath->PathPoints.Last();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagara, CursorHitResult.ImpactPoint);
}

//TODO: Set Physical Material for Wall and Ground
void AAuraPlayerController::UpdateAim()
{	/*FVector2D MousePos; if (!GetMousePosition(MousePos.X, MousePos.Y)) return; GetHitResultAtScreenPosition();*/
	/*const bool bHit =*/ GetHitResultUnderCursor(ECC_AuraTrace_Mouse, false, CursorHitResult);
	/*DrawDebugLineTraceSingle(GetWorld(), CursorHitResult.TraceStart, CursorHitResult.TraceEnd,
		EDrawDebugTrace::ForDuration, bHit, CursorHitResult, FColor::Red, FColor::Green, 5.f);*/

	const FVector CharacterLocation = GetPawn()->GetActorLocation();
	/*
	 * We use a point at the same plane as character location:
	 * - To improve click-accuracy from player's POV because projectiles are spawned near that plane
	 * - Still get an accurate mouse-to-character direction even if character is on different altitude than HitResult ground (or none)
	 */
	FVector Intersection; float TIntersection;
	UKismetMathLibrary::LinePlaneIntersection_OriginNormal(CursorHitResult.TraceStart, CursorHitResult.TraceEnd,
		CharacterLocation, FVector::UpVector, TIntersection, Intersection);
	AuraPawn->AimDirection = (Intersection - CharacterLocation).GetSafeNormal();
	ServerSetCharacterAimDirection(AuraPawn->AimDirection);
}

void AAuraPlayerController::CursorTick()
{
	//CurrentCursorHitActor = CursorHitResult.GetActor(); // cast to IEnemyInterface, nullptr if can't (i.e. Floor -> nullptr)
	if (!CursorHitResult.bBlockingHit) return;
	AAuraEnemy* LastEnemy = CursorHitEnemy;
	CursorHitEnemy = Cast<AAuraEnemy>(CursorHitResult.GetActor());
	if (CursorHitEnemy != LastEnemy)
	{
		if (LastEnemy) LastEnemy->UnHighlightActor();
		if (CursorHitEnemy) CursorHitEnemy->HighlightActor();
	}

	if (CursorHitEnemy) return; // Has valid Hit to end here
	TArray<FHitResult> Hits;
	UAuraAbilityLibrary::TraceByChannel(this, CursorHitResult.TraceStart, CursorHitResult.TraceEnd,
		{GetPawn()}, EDrawDebugTrace::None, Hits, {ECC_Pawn}, 50.f, false);
	float NearestDistance = UE_BIG_NUMBER;
	for (const FHitResult& Hit : Hits)
	{
		AAuraEnemy* HitEnemy = Cast<AAuraEnemy>(Hit.GetActor());
		if (HitEnemy == nullptr) continue;
		const float Distance = FMath::PointDistToLine(HitEnemy->GetActorLocation(),
			CursorHitResult.TraceEnd - CursorHitResult.TraceStart, CursorHitResult.TraceStart);
		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			CursorHitEnemy = HitEnemy;
		}
	}
	// if (AActor* Nearest = UGameplayStatics::FindNearestActor(Intersection, HitActors, NearestDistance))
	if (CursorHitEnemy) CursorHitEnemy->HighlightActor();
	/*GetWorld()->OverlapMultiByObjectType();*/
}

// Change on server for UPROPERTY(Replicated/ReplicatedUsing) to work, else use AbilityTask_AimData
void AAuraPlayerController::ServerSetCharacterAimDirection_Implementation(const FVector_NetQuantizeNormal& Aim)
{
	AuraPawn->AimDirection = Aim;
}
