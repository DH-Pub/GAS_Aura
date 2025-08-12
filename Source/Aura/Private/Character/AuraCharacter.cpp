// Copyright Hung


#include "Character/AuraCharacter.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/CharacterWidgetController.h"

AAuraCharacter::AAuraCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeRotation(FRotator(-45., 0., 0.));
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = 800.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 25.f;
	SpringArm->bInheritPitch = SpringArm->bInheritRoll = SpringArm->bInheritYaw = false;
	SpringArm->bDoCollisionTest = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	CameraCapsule = CreateDefaultSubobject<UCapsuleComponent>("CameraCapsule");
	CameraCapsule->SetupAttachment(Camera);
	CameraCapsule->SetRelativeLocationAndRotation(FVector(370., 0., 0.), FRotator(90., 0., 0.));
	CameraCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	CameraCapsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagara");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	LevelUpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("LevelUpWidget");
	LevelUpWidgetComponent->SetupAttachment(GetRootComponent());
}


void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// for the server
	InitAbilityActorInfo();
	AddCharacterStartupAbilities();
}
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// for the client
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetCharacterLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::MulticastLevelUpEffects_Implementation(int32 Level)
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FRotator CameraRotation = Camera->GetComponentRotation();
		LevelUpNiagaraComponent->SetWorldRotation(FRotator(CameraRotation.Pitch * -1., CameraRotation.Yaw + 180., 0.));
		LevelUpNiagaraComponent->Activate(true);
	}
	if (CharacterWidgetController)
	{
		CharacterWidgetController->OnLevelUpDelegate.Broadcast(Level);
	}
}

void AAuraCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UAuraUserWidget* LevelUpWidget = Cast<UAuraUserWidget>(LevelUpWidgetComponent->GetUserWidgetObject()))
	{
		UAuraWidgetController::CreateOrGetWidgetController(this, CharacterWidgetController, CharacterWidgetClass,
			FWidgetControllerParams(
				Cast<AAuraPlayerController>(GetController()), GetPlayerState<AAuraPlayerState>(),
				AbilitySystemComponent, AttributeSet));
		LevelUpWidget->SetWidgetController(CharacterWidgetController);
	}
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AbilitySystemComponent = AuraPlayerState->GetAuraAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState,this); // Set Owner and Avatar
	AbilitySystemComponent->AbilityActorInfoSet();
	AttributeSet = AuraPlayerState->GetAuraAttributeSet();
	
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = AuraPC->GetHUD<AAuraHUD>()) // Only Local Client can get HUD
		{
			AuraHUD->InitOverlay(FWidgetControllerParams(AuraPC, AuraPlayerState, AbilitySystemComponent, AttributeSet));
		}
	}

	// Initialize Default Attributes
	constexpr float Level = 1.f;
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, this, CharacterClass, Level, AbilitySystemComponent);
	}
}
