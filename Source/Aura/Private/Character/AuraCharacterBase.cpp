// Copyright Hung


#include "Character/AuraCharacterBase.h"

#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
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
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetCollisionObjectType(ECC_PhysicsBody);
	Weapon->SetCollisionResponseToAllChannels(ECR_Ignore);

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

void AAuraCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (CombatTarget) TargetLocation = CombatTarget->GetActorLocation();
	if (bTracking)
	{
		TEnumAsByte<EOutcome> Outcome = Failure;
		UAuraAbilitySystemLibrary::YawActorToLocation(Outcome, this, TargetLocation, DeltaSeconds,
			GetCharacterMovement()->RotationRate.Yaw * 2);
		// const FRotator ResultRot = UKismetMathLibrary::RInterpTo_Constant(
		// 	GetCapsuleComponent()->GetComponentRotation(),
		// 	UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatTarget->GetActorLocation()),
		// 	DeltaSeconds, GetCharacterMovement()->RotationRate.Yaw * 2);
		// GetCapsuleComponent()->SetWorldRotation(ResultRot);
	}
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	if (Weapon->GetSkeletalMeshAsset())
	{
		Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		Weapon->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	bIsDead = true;
	Dissolve();
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
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
	/*const APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (LocalPlayerController && LocalPlayerController == SourceController)// check if damage dealer is the local player*/
	{
		BP_ShowDamageNumber(HitLocation, Damage, bBlocked, bCrit);
	}
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

FTaggedMontage AAuraCharacterBase::GetRandomAttackMontage_Implementation()
{
	if (AttackMontages.IsEmpty()) return FTaggedMontage();
	return AttackMontages[FMath::RandRange(0, AttackMontages.Num() - 1)];
}
FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	if (IsValid(Weapon->GetSkeletalMeshAsset()) && MontageTag.MatchesTagExact(AuraGameplayTags::CombatSocket_Weapon))
	{return Weapon->GetSocketLocation("Attack_Socket");}
	if (MontageTag.MatchesTagExact(AuraGameplayTags::CombatSocket_LeftHand))
	{return GetMesh()->GetSocketLocation("Hand_L_Socket");}
	if (MontageTag.MatchesTagExact(AuraGameplayTags::CombatSocket_RightHand))
	{return GetMesh()->GetSocketLocation("Hand_R_Socket");}
	if (MontageTag.MatchesTagExact(AuraGameplayTags::CombatSocket_Tail))
	{return GetMesh()->GetSocketLocation("Tail_Socket");}
	return GetActorLocation();
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag) return TaggedMontage;
	}
	return FTaggedMontage();
}

void AAuraCharacterBase::AddCharacterStartupAbilities() const
{
	if (!HasAuthority()) return;
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	// Grant ability from server
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassives(StartupPassives);
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
