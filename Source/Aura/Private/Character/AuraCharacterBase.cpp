// Copyright Hung


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "AuraAbilityLibrary.h"
#include "AuraEffectTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Ability/DamageAbility.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Aura/Aura.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Mouse, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetRelativeRotation(FRotator(0., -90., 0.));
	// Dedicated servers don't render the meshes
	// Skeletal meshes do not update their sockets or bones while not being rendered by default on the server part
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	// GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	// avoid
	// GetCharacterMovement()->bUseRVOAvoidance = true;
	// GetCharacterMovement()->AvoidanceConsiderationRadius = 100.f;
	bUseControllerRotationPitch = bUseControllerRotationRoll = bUseControllerRotationYaw = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName(TEXT("WeaponHandSocket")));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetCollisionObjectType(ECC_PhysicsBody);
	Weapon->SetCollisionResponseToAllChannels(ECR_Ignore);

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuff");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = AuraGameplayTags::Debuff_Type_Burn;
}

void AAuraCharacterBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (CombatTarget) AimDirection = CombatTarget->GetActorLocation() - GetActorLocation();
	if (bTracking)
	{
		UAuraAbilityLibrary::YawActorToRotation(this, AimDirection, DeltaSeconds,
			GetCharacterMovement()->RotationRate.Yaw * 2);
	}
}

void AAuraCharacterBase::GetRandomAttackMontage(FTaggedMontage& TaggedMontage)
{
	TaggedMontage = AttackMontages[FMath::RandRange(0, AttackMontages.Num() - 1)];
}
void AAuraCharacterBase::GetTaggedMontageByTag(const FGameplayTag& MontageTag, FTaggedMontage& TaggedMontage)
{
	for (const FTaggedMontage& Montage : AttackMontages)
	{
		if (Montage.MontageTag == MontageTag) {TaggedMontage = Montage; return;}
	}
}

FVector AAuraCharacterBase::GetCombatSocketLocation(const ECombatSocket SocketEnum)
{
	switch (SocketEnum)
	{
	case ECombatSocket::Weapon: return Weapon->GetSocketLocation("Attack_Socket");
	case ECombatSocket::LeftHand: return GetMesh()->GetSocketLocation("Hand_L_Socket");
	case ECombatSocket::RightHand: return GetMesh()->GetSocketLocation("Hand_R_Socket");
	case ECombatSocket::Tail: return GetMesh()->GetSocketLocation("Tail_Socket");
	default: return GetActorLocation();
	}
}

// Define in .cpp or we need to #include "AbilitySystem/AuraAbilitySystemComponent.h" in header
UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const {return AbilitySystemComponent;}

int32 AAuraCharacterBase::GetCharacterLevel_Implementation() const {return 1;}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& HitImpulse)
{
	if (Weapon->GetSkeletalMeshAsset())
	{	// Drop Weapon
		Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Weapon->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	} // Enable Ragdoll
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	if (!HitImpulse.IsNearlyZero())
	{
		if (Weapon->GetSkeletalMeshAsset()) Weapon->AddImpulseToAllBodiesBelow(HitImpulse, NAME_None, true);
		GetMesh()->AddImpulseToAllBodiesBelow(HitImpulse, NAME_None, true);
	}
	Dissolve();
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	BurnDebuffComponent->DisableNiagara(AbilitySystemComponent);
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(AuraGameplayTags::Character_State_Death);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->RotationRate = BaseRotationRate;
}

// Called in PossessedBy, which is called only on server or standalone
void AAuraCharacterBase::AddCharacterStartupAbilities() const
{
	if (!HasAuthority()) return; // Grant ability from server
	AbilitySystemComponent->AddCharacterAbilities(StartupAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(MeshDissolveMI))
	{
		UMaterialInstanceDynamic* MIDynamic = UMaterialInstanceDynamic::Create(MeshDissolveMI, this);
		GetMesh()->SetMaterial(0, MIDynamic);
		StartDissolveTimeline(MIDynamic);
	}
	if (Weapon->GetSkeletalMeshAsset() && IsValid(WeaponDissolveMI))
	{
		UMaterialInstanceDynamic* MIDynamic = UMaterialInstanceDynamic::Create(WeaponDissolveMI, this);
		Weapon->SetMaterial(0, MIDynamic);
		StartWeaponDissolveTimeline(MIDynamic);
	}
}
