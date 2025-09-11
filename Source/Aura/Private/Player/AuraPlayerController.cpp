// Copyright Hung


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Character/AuraCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "Kismet/GameplayStatics.h"
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

	switch (MouseMovementState)
	{
	case Stop: break;
	case AutoMove:
		if (GetPawn())
		{
			const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
				GetPawn()->GetActorLocation(), ESplineCoordinateSpace::World);
			// FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
			FVector Direction = Spline->FindTangentClosestToWorldLocation(GetPawn()->GetActorLocation(), ESplineCoordinateSpace::World);
			Direction += LocationOnSpline - GetPawn()->GetActorLocation();
			GetPawn()->AddMovementInput(Direction);

			const float DistanceToDestinationSquared = (LocationOnSpline - AutoMoveDestination).SizeSquared();
			if (DistanceToDestinationSquared < AutoRunAcceptanceRadius * AutoRunAcceptanceRadius)
			{
				Spline->ClearSplinePoints();
				MouseMovementState = Stop;
			}
		}
		break;
	case HoldMove:
		if (GetPawn() == nullptr) return;
		/*const FVector WorldDirection = (CursorHitResult.ImpactPoint - GetPawn()->GetActorLocation()).GetSafeNormal();
		GetPawn()->AddMovementInput(WorldDirection);*/

		FVector2D CharacterToScreen;
		ProjectWorldLocationToScreen(GetPawn()->GetActorLocation(), CharacterToScreen);
		FVector2D MouseToScreen;
		GetMousePosition(MouseToScreen.X, MouseToScreen.Y);
		FVector2D ControlDirection = MouseToScreen - CharacterToScreen;
		ControlDirection.Y *= -1.f;  // Y-axis direction in input is reverse for screen vector

		const FRotator Rotation = CameraManager->GetCameraRotation();
		const FRotator YawRotation(0., Rotation.Yaw, 0.);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		GetPawn()->GetMovementComponent()->AddInputVector(ForwardDirection * ControlDirection.Y + RightDirection * ControlDirection.X);
		break;
	}
}

void AAuraPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	SetCameraComponent();
}
UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraASC()
{
	if (AbilitySystemComponent == nullptr)
	{
		AbilitySystemComponent = Cast<AAuraCharacterBase>(GetPawn())->GetAuraAbilitySystemComponent();
		// AbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AbilitySystemComponent;
}


void AAuraPlayerController::CursorTrace()
{
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

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
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
	
	UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	AuraInputComponent->BindAbilityActions(InputConfig, InputMappingContext, InputSystem,this, &AAuraPlayerController::ControllerInputTrigger);
}


// ======================================================================================================================================
#pragma region Occlusion
void AAuraPlayerController::SetCameraComponent()
{
	// APawn::Controller might replicate before AController::Pawn so GetPawn() might be nullptr
	if (!IsLocalController()) return;
	if (AAuraCharacter* AuraCharacter = Cast<AAuraCharacter>(GetPawn()))
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

	// const FRotator Rotation = GetControlRotation(); // Camera->bUsePawnControlRotation has to be true for this to work
	const FRotator Rotation = CameraManager->GetCameraRotation();
	const FRotator YawRotation(0., Rotation.Yaw, 0.);

	// Camera to player leveled to the ground
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	GetPawn()->GetMovementComponent()->AddInputVector(ForwardDirection * InputAxisVector.Y + RightDirection * InputAxisVector.X);
	/*GetPawn()->AddMovementInput(ForwardDirection, InputAxisVector.Y);
	GetPawn()->AddMovementInput(RightDirection, InputAxisVector.X);*/
}

void AAuraPlayerController::ControllerInputTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag* InputTag,
	const TObjectPtr<UInputAction> InputAction)
{
	if (GetAuraASC())
	{
		AbilitySystemComponent->AbilityInputTagTrigger(TriggerEvent, *InputTag, InputAction);
	}
}
