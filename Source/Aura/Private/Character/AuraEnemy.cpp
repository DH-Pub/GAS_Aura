// Copyright Hung


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "AbilitySystem/Data/CharacterClassDataAsset.h"
#include "AI/AuraAIController.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/EnemyWidgetController.h"

AAuraEnemy::AAuraEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal); // Save bandwidth

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	// No need to SpawnDefaultController()
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	SetNetUpdateFrequency(5); // Slow AI so no need for much update
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
	// HealthBar->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	HealthBar->SetVisibility(false);
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	InitAuraCharacter();
}

void AAuraEnemy::InitAuraCharacter()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	UAuraWidgetController::CreateOrGetWidgetController<UEnemyWidgetController>(HealthBarController, AbilitySystemComponent);
	Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject())->SetWidgetController(HealthBarController);

	if (const UCharacterClassDataAsset* ClassData = UCharacterClassDataAsset::GetFromGameMode(this))
	{
		ClassData->InitializeDefaultAttributes(CharacterClass, Level, AbilitySystemComponent);
		ClassData->GiveStartupAbilities(this);
	}
}
