// Copyright Hung


#include "Character/AuraCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Mouse, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0., 540., 0.);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	// avoid
	// GetCharacterMovement()->bUseRVOAvoidance = true;
	// GetCharacterMovement()->AvoidanceConsiderationRadius = 100.f;
	bUseControllerRotationPitch = bUseControllerRotationRoll = bUseControllerRotationYaw = false;
}

void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}
void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	Dissolve();
}

void AAuraCharacterBase::ShowDamageNumber_Implementation(const AController* SourceController, const FVector& HitLocation,
	const float Damage, const bool bBlocked, const bool bCrit)
{
	/*if (DamageTextComponentClass)
	{
		UDamageTextComponent* DmgTxt = NewObject<UDamageTextComponent>(this, DamageTextComponentClass);
		DmgTxt->RegisterComponent();
		// DmgTxt->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); // to set location
		// DmgTxt->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // to not when character move
		DmgTxt->SetWorldLocation(GetActorLocation());
		DmgTxt->BP_SetDamageText(Damage);
	}*/
	const APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (LocalPlayerController && LocalPlayerController == SourceController)// check if damage dealer is the local player
	{
		BP_ShowDamageNumber(HitLocation, Damage, bBlocked, bCrit);
	}
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	
	if (HasAuthority()) AuraASC->AddCharacterAbilities(StartupAbilities); // Grant ability from server
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(MeshDissolveMI))
	{
		UMaterialInstanceDynamic* MIDynamic = UMaterialInstanceDynamic::Create(MeshDissolveMI, this);
		GetMesh()->SetMaterial(0, MIDynamic);
		StartDissolveTimeline(MIDynamic);
	}
	if (IsValid(WeaponDissolveMI))
	{
		UMaterialInstanceDynamic* MIDynamic = UMaterialInstanceDynamic::Create(WeaponDissolveMI, this);
		Weapon->SetMaterial(0, MIDynamic);
		StartWeaponDissolveTimeline(MIDynamic);
	}
}
