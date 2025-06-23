// Copyright Hung


#include "Character/AuraEnemy.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
// #include "BehaviorTree/BehaviorTree.h"
// #include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "AI/AuraAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	// No need to SpawnDefaultController()
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority())
	{
		AuraAIController = Cast<AAuraAIController>(NewController);
		// AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		// AuraAIController->RunBehaviorTree(BehaviorTree);
		// AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
		// AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
	} // Behavior Tree (DEPRECATED)
}

void AAuraEnemy::HighlightActor()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(GCustom_Depth_Red);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(GCustom_Depth_Red);
}

void AAuraEnemy::UnHighlightActor()
{
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();
	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
	}

	if (UAuraUserWidget* Widget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		const FWidgetControllerParams WCParams(nullptr, nullptr, AbilitySystemComponent, AttributeSet);
		UAuraWidgetController::CreateOrGetWidgetController(this, HealthBarController, HealthBarControllerClass, WCParams);
		Widget->SetWidgetController(HealthBarController);
		HealthBarController->BroadcastInitialValues();
	}

	AbilitySystemComponent->RegisterGameplayTagEvent(AuraGameplayTags::Effects_HitReact,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraEnemy::HitReactChanged);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
	{
		UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, this, CharacterClass, Level, AbilitySystemComponent);
	}
}

void AAuraEnemy::Die()
{
	Super::Die();
	SetLifeSpan(LifeSpan);
	if (HasAuthority())
	{
		// Disable StateTree on death
		AuraAIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	} 
}
void AAuraEnemy::MulticastHandleDeath_Implementation()
{
	Super::MulticastHandleDeath_Implementation();
	HealthBar->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	// HealthBar->SetVisibility(false);
}

void AAuraEnemy::HitReactChanged(const FGameplayTag CallbackTag, const int NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	if (HasAuthority())
	{
		// AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	} // BehaviorTree (DEPRECATED)
}

void AAuraEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bTracking && CombatTarget)
	{
		TEnumAsByte Outcome = Failure;
		UAuraAbilitySystemLibrary::YawActorToLocation(Outcome, this, CombatTarget->GetActorLocation(), DeltaSeconds,
			GetCharacterMovement()->RotationRate.Yaw * 2);
		// const FRotator ResultRot = UKismetMathLibrary::RInterpTo_Constant(
		// 	GetCapsuleComponent()->GetComponentRotation(),
		// 	UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation()),
		// 	DeltaSeconds, GetCharacterMovement()->RotationRate.Yaw * 2);
		// GetCapsuleComponent()->SetWorldRotation(ResultRot);
	}
}
