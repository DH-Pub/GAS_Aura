// Copyright Hung


#include "Character/AuraAnimInstance.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/AuraCharacterBase.h"
#include "Character/Component/AuraMovementComponent.h"
#include "Misc/DataValidation.h"

UAuraAnimInstance::UAuraAnimInstance()
{
}

void UAuraAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation(); // BlueprintInitializeAnimation() is called after this

	AuraCharacter = Cast<AAuraCharacterBase>(GetOwningActor());
	if (!AuraCharacter) return;
	MovementComponent = AuraCharacter->GetAuraMovementComponent();

	if (AuraCharacter->GetAbilitySystemComponent())
	{
		InitializeWithAbilitySystem(AuraCharacter->GetAbilitySystemComponent());
	}
}

void UAuraAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

}

void UAuraAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

}

void UAuraAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC)
	GameplayTagPropertyMap.Initialize(this, ASC);
}


#if WITH_EDITOR
EDataValidationResult UAuraAnimInstance::IsDataValid(class FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return Context.GetNumErrors() > 0 ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
