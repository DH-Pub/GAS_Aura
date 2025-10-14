// Copyright Hung


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Character/AuraPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interface/EnemyInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Splines");
}

void AAuraPlayerController::PlayerTick(const float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (AuraPawn == nullptr) return;
	CursorTrace();
	const FVector CharacterLocation = GetPawn()->GetActorLocation();

	FVector Intersection; float TIntersection;
	UKismetMathLibrary::LinePlaneIntersection_OriginNormal(CursorHitResult.TraceStart, CursorHitResult.TraceEnd,
		CharacterLocation, GetPawn()->GetActorUpVector(),
		TIntersection, Intersection);

	// Direction by mouse
	FVector2D CharacterToScreen; UGameplayStatics::ProjectWorldToScreen(this, CharacterLocation, CharacterToScreen);
	FVector2D MouseToScreen; GetMousePosition(MouseToScreen.X, MouseToScreen.Y);
	AuraPawn->AimDirection = Intersection - CharacterLocation;

	switch (MouseMovementState)
	{
	case Stop: break;
	case AutoMove:
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			GetPawn()->GetActorLocation(), ESplineCoordinateSpace::World);
		// FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		FVector Direction = Spline->FindTangentClosestToWorldLocation(GetPawn()->GetActorLocation(), ESplineCoordinateSpace::World);
		Direction += LocationOnSpline - GetPawn()->GetActorLocation();
		GetPawn()->AddMovementInput(Direction);
		if ((LocationOnSpline - AutoMoveDestination).SizeSquared() < AutoRunAcceptanceRadius * AutoRunAcceptanceRadius)
		{
			Spline->ClearSplinePoints();
			MouseMovementState = Stop;
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
	CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	SetCameraComponent();
}


void AAuraPlayerController::CursorTrace()
{	// Cursor highlight
	GetHitResultUnderCursor(ECC_Mouse, false, CursorHitResult);
	if (!CursorHitResult.bBlockingHit) return;

	const TScriptInterface<IEnemyInterface> LastActor = CurrentCursorHitActor;
	CurrentCursorHitActor = CursorHitResult.GetActor(); // cast to IEnemyInterface, nullptr if can't (i.e. Floor -> nullptr)
	if (CurrentCursorHitActor != LastActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (CurrentCursorHitActor) CurrentCursorHitActor->HighlightActor();
	}
}

void AAuraPlayerController::BeginPlay()
{
	check(InputMappingContext); // check/verify/ensure
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSystem->AddMappingContext(InputMappingContext, 0);
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
{
	Super::SetupInputComponent();

	AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Ongoing, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityActions(InputConfig, InputMappingContext,this, &AAuraPlayerController::ControllerInputTrigger);
}


// ======================================================================================================================================
#pragma region Occlusion
void AAuraPlayerController::SetCameraComponent()
{
	// APawn::Controller might replicate before AController::Pawn so GetPawn() might be nullptr
	if (!IsLocalController()) return;
	if (AAuraPlayer* AuraCharacter = GetPawn<AAuraPlayer>())
	{
		/*ActiveSpringArm = Cast<USpringArmComponent>(AuraCharacter->GetComponentByClass(USpringArmComponent::StaticClass()));
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
			// Material.Value->SetScalarParameterValue(FName("Fade"), 1.f);
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
	MouseMovementState = Stop;
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	//TODO: No need to run this every tick because this project camera is static
	// const FRotator Rotation = GetControlRotation(); // Camera->bUsePawnControlRotation has to be true for this to work
	const FRotator Rotation = CameraManager->GetCameraRotation();
	const FRotator YawRotation(0., Rotation.Yaw, 0.);
	// Camera to player leveled to the ground
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	GetPawn()->GetMovementComponent()->AddInputVector(ForwardDirection * InputAxisVector.Y + RightDirection * InputAxisVector.X);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AAuraPlayerController::ControllerInputTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag* InputTag,
	UInputAction* InputAction)
{
	if (AuraPawn == nullptr) return;
	AuraPawn->GetAuraAbilitySystemComponent()->AbilityInputTagTrigger(TriggerEvent, *InputTag, InputAction);
}
