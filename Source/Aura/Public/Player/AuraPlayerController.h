// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

enum class ETriggerEvent : uint8;
class UInputAction;
struct FInputActionValue;

USTRUCT(BlueprintType)
struct FCameraOccludedStaticMesh
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> DefaultMaterials;
	FCameraOccludedStaticMesh(){}
};

enum EMovementState : uint8
{
	Stop, AutoMove, HoldMove
};
/**
 * For multiplayer: Project Settings -> Engine/Navigation System -> Allow Client Side Navigation
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override; // Processes player input
	virtual void SetPawn(APawn* InPawn) override;

	UFUNCTION(BlueprintGetter)
	FORCEINLINE FHitResult& GetCursorHitResult() { return CursorHitResult; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class USplineComponent> Spline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Aura|Input")
	float AutoRunAcceptanceRadius = 25.f;

	UPROPERTY()
	TObjectPtr<class AAuraCharacterBase> AuraPawn;
	UPROPERTY()
	TObjectPtr<class UAuraAbilitySystemComponent> AuraASC;

	UPROPERTY()
	TObjectPtr<class AAuraEnemy> CursorHitEnemy; // UPROPERTY() TScriptInterface<class ICombatInterface> CursorHit;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;


protected:
// ==========================================================================================================
#pragma region Occlusion
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCapsuleComponent> CameraCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aura")
	TObjectPtr<UMaterialInterface> FadeMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	float FadeIntensity = .25f;
	UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>, FCameraOccludedStaticMesh> OccludedMeshes; // <Mesh, >

	void SetCameraComponent();
	UFUNCTION()
	void OnCameraCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnCameraCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion
// =====================================================================================================================


private: //TODO: Move all below to AuraHeroComponent
	UPROPERTY(EditDefaultsOnly, Category="Aura|Input")
	TObjectPtr<const class UAuraInputDataAsset> AuraInputDA;

	void Move(const FInputActionValue& InputActionValue);

#pragma region Click Movement Nav =================================
	EMovementState MovementState = Stop;
	float MoveHoldTime = 0.f;
	float HoldTimeThreshold = .4f;
	void MoveMouseTriggered(const FInputActionValue& InputActionValue);
	void MoveMouseComplete(const FInputActionValue& InputActionValue);
	UPROPERTY(EditDefaultsOnly, Category="Aura|Mouse")
	bool bDrawNavBox = false;
	UPROPERTY(EditDefaultsOnly, Category="Aura|Mouse")
	FVector NavExtent = FVector(500.f, 500.f, 600.f);
#pragma endregion

	UPROPERTY(EditDefaultsOnly, Category="Aura|Click")
	TObjectPtr<class UNiagaraSystem> ClickNiagara;

	UPROPERTY(BlueprintGetter=GetCursorHitResult, meta=(AllowPrivateAccess))
	FHitResult CursorHitResult;
	void UpdateAim();
	void CursorTick(); // Cursor HitResult

	UFUNCTION(Server, Reliable)
	void ServerSetCharacterAimDirection(const FVector_NetQuantizeNormal& Aim);
};
