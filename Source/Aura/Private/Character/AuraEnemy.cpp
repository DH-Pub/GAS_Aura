// Copyright Hung


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "BrainComponent.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "AI/AuraAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/EnemyWidgetController.h"

AAuraEnemy::AAuraEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Save bandwidth

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
	}
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

void AAuraEnemy::MulticastHandleDeath_Implementation(const FVector& HitImpulse)
{
	Super::MulticastHandleDeath_Implementation(HitImpulse);
	HealthBar->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	// HealthBar->SetVisibility(false);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAuraCharacter();

	UAuraUserWidget* Widget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject());
	UAuraWidgetController::CreateOrGetWidgetController(this, this, HealthBarController, HealthBarControllerClass);
	Widget->SetWidgetController(HealthBarController);
}

void AAuraEnemy::InitAuraCharacter()
{
	AbilitySystemComponent->InitAuraASC(this, this);

	if (const UCharacterClassDataAsset* ClassData = UCharacterClassDataAsset::GetFromGameMode(this))
	{
		ClassData->InitializeDefaultAttributes(CharacterClass, Level, AbilitySystemComponent);
		ClassData->GiveStartupAbilities(this);
	}
}
