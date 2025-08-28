// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraInputComponent;
enum class ETriggerEvent : uint8;
class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputAction;
class IEnemyInterface;
class UInputMappingContext;
struct FInputActionValue;

USTRUCT(BlueprintType)
struct FCameraOccludedStaticMesh
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> DefaultMaterials;
	FCameraOccludedStaticMesh(){}
};

enum EMouseMovementState : uint8
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

	UPROPERTY()
	TObjectPtr<UAuraInputComponent> AuraInputComponent;
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UAuraAbilitySystemComponent* GetAuraASC();
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FHitResult& GetCursorHitResult() { return CursorHitResult; }

	EMouseMovementState MouseMovementState = Stop;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class USplineComponent> Spline;
	FVector AutoMoveDestination = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Default|Input")
	float AutoRunAcceptanceRadius = 25.f;
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

// =========================================================================================================================================
#pragma region Occlusion
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCapsuleComponent> CameraCapsule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	TObjectPtr<UMaterialInterface> FadeMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Default")
	float FadeIntensity = .25f;
	UPROPERTY()
	TMap<TObjectPtr<UStaticMeshComponent>, FCameraOccludedStaticMesh> OccludedMeshes; // <Mesh, >

	void SetCameraCapsule();
	UFUNCTION()
	void OnCameraCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnCameraCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion
// ==========================================================================================================================================


private:
	UPROPERTY(EditDefaultsOnly, Category = "Default|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	//TODO Switch
	UPROPERTY(EditDefaultsOnly, Category = "Default|Input")
	TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& InputActionValue);

	UPROPERTY(BlueprintGetter=GetCursorHitResult, meta=(AllowPrivateAccess))
	FHitResult CursorHitResult;
	void CursorTrace();
	UPROPERTY(BlueprintReadWrite, meta=(AllowPrivateAccess))
	TScriptInterface<IEnemyInterface> CurrentCursorHitActor;

	void ControllerInputTrigger(const ETriggerEvent TriggerEvent, const FGameplayTag* InputTag, TObjectPtr<UInputAction> InputAction);

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;
};
