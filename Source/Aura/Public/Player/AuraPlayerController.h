// Copyright Hung

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class AAuraCharacterBase;
enum class ETriggerEvent : uint8;
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
	
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<class UAuraInputConfig> InputConfig;
	
	UFUNCTION(BlueprintGetter)
	FORCEINLINE FHitResult& GetCursorHitResult() { return CursorHitResult; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class USplineComponent> Spline;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Default|Input")
	float AutoRunAcceptanceRadius = 25.f;
	
	UPROPERTY()
	TObjectPtr<AAuraCharacterBase> AuraPawn;
	UPROPERTY()
	TObjectPtr<class UAuraAbilitySystemComponent> AuraASC;
	
	UPROPERTY()
	TObjectPtr<class AAuraEnemy> CursorHitEnemy;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	
// ==========================================================================================================
#pragma region Occlusion
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCapsuleComponent> CameraCapsule;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	TObjectPtr<UMaterialInterface> FadeMaterial;
	UPROPERTY(EditDefaultsOnly, Category="Default")
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


private:
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& InputActionValue);
	
#pragma region Click Movement Nav =================================
	UPROPERTY(EditDefaultsOnly, Category="Default|Input")
	TObjectPtr<UInputAction> MoveMouseAction;
	EMovementState MovementState = Stop;
	float MoveHoldTime = 0.f;
	float HoldTimeThreshold = .4f;
	UPROPERTY()
	TObjectPtr<class UNavigationSystemV1> NavSystem;
	void MoveMouseTriggered(const FInputActionValue& InputActionValue);
	void MoveMouseComplete(const FInputActionValue& InputActionValue);
	UPROPERTY(EditDefaultsOnly, Category="Default|Mouse")
	bool bDrawNavBox = false;
	UPROPERTY(EditDefaultsOnly, Category="Default|Mouse")
	FVector NavExtent = FVector(300.f, 300.f, 600.f);
#pragma endregion
	
	UPROPERTY(EditDefaultsOnly, Category="Default|Click")
	TObjectPtr<class UNiagaraSystem> ClickNiagara;
	
	UPROPERTY(BlueprintGetter=GetCursorHitResult, meta=(AllowPrivateAccess))
	FHitResult CursorHitResult;
	void CursorTick(); // Cursor HitResult
	
	void PlayerInputPressed(const FGameplayTag InputTag);
	void PlayerInputReleased(const FGameplayTag InputTag);
	
	UFUNCTION(Server, Reliable)
	void ServerSetCharacterAimDirection(const FVector_NetQuantizeNormal& Aim);
};
