// Copyright Hung


#include "Character/AuraPlayer.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/Ability/AttributesEventAbility.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "Components/WidgetComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/CharacterWidgetController.h"

AAuraPlayer::AAuraPlayer()
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
	CameraCapsule->SetCapsuleHalfHeight(350.f);
	CameraCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	CameraCapsule->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagara");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

	LevelUpWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("LevelUpWidget");
	LevelUpWidgetComponent->SetupAttachment(GetRootComponent());
	LevelUpWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	LevelUpWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LevelUpWidgetComponent->SetDrawAtDesiredSize(true);
}


void AAuraPlayer::PossessedBy(AController* NewController) // SERVER
{
	Super::PossessedBy(NewController);
	InitAuraCharacter();
	AddCharacterStartupAbilities();
}
void AAuraPlayer::OnRep_PlayerState() // CLIENT
{
	Super::OnRep_PlayerState();
	InitAuraCharacter();
}

int32 AAuraPlayer::GetCharacterLevel_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraPlayer::MulticastLevelUpEffects_Implementation(const int32 Level)
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FRotator CameraRotation = Camera->GetComponentRotation();
		LevelUpNiagaraComponent->SetWorldRotation(FRotator(-CameraRotation.Pitch, CameraRotation.Yaw + 180., 0.));
		LevelUpNiagaraComponent->Activate(true);
	}
	CharacterWC->OnLevelUpDelegate.Broadcast(Level);
}

void AAuraPlayer::BeginPlay()
{
	Super::BeginPlay();

	UAuraUserWidget* LevelUpWidget = Cast<UAuraUserWidget>(LevelUpWidgetComponent->GetUserWidgetObject());
	UAuraWidgetController::CreateOrGetWidgetController(this, this, CharacterWC, CharacterWidgetClass);
	LevelUpWidget->SetWidgetController(CharacterWC);
}

void AAuraPlayer::InitAuraCharacter()
{
	AAuraPlayerState* AuraPS = GetPlayerState<AAuraPlayerState>(); // Every Client has access to every PlayerState
	AbilitySystemComponent = AuraPS->GetAuraAbilitySystemComponent();
	AbilitySystemComponent->InitAuraASC(AuraPS, this);
	AttributeSet = AuraPS->GetAuraAttributeSet();
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(GetController()))	// Server and local client
	{	// Only Local Client can get HUD
		if (AAuraHUD* AuraHUD = AuraPC->GetHUD<AAuraHUD>()) AuraHUD->InitAuraHUD(AuraPC, AuraPS, this);
	}
	// Initialize Default Attributes
	constexpr float Level = 1.f;
	if (const UCharacterClassDataAsset* ClassData = UCharacterClassDataAsset::GetFromGameMode(this))
	{
		ClassData->InitializeDefaultAttributes(CharacterClass, Level, AbilitySystemComponent);
		ClassData->GiveStartupAbilities(this);
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UAttributesEventAbility::StaticClass(), 1));
	}
}
