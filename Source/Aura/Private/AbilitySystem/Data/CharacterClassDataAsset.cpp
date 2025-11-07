// Copyright Hung


#include "AbilitySystem/Data/CharacterClassDataAsset.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Ability/AuraGameplayAbility.h"
#include "Character/AuraCharacterBase.h"
#include "Game/AuraGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"

const UCharacterClassDataAsset* UCharacterClassDataAsset::GetFromGameMode(const UObject* WorldContextObject)
{
	if (const AAuraGameMode* AuraGameMode = Cast<AAuraGameMode>(UGameplayStatics::GetGameMode(WorldContextObject)))
	{
		return AuraGameMode->CharacterClassData;
	}
	return nullptr;
}

/* Make sure to check HasAuthority before calling this */
void UCharacterClassDataAsset::InitializeDefaultAttributes(const ECharacterClass CharacterClass, const float Level,
	UAbilitySystemComponent* ASC) const
{
	const FCharacterClassDefaultInfo* ClassDefaultInfo = GetClassDefaultInfo(CharacterClass);
	const FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();

	const FGameplayEffectSpecHandle PrimaryAttrSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo->PrimaryAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttrSpecHandle.Data);

	const FGameplayEffectSpecHandle SecondaryAttrSpecHandle = ASC->MakeOutgoingSpec(SecondaryAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttrSpecHandle.Data);

	const FGameplayEffectSpecHandle VitalAttrSpecHandle = ASC->MakeOutgoingSpec(VitalAttributes, Level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttrSpecHandle.Data);
}

/* Make sure to check HasAuthority() before calling this */
void UCharacterClassDataAsset::GiveStartupAbilities(const AAuraCharacterBase* AuraCharacter) const
{
	UAuraAbilitySystemComponent* ASC = AuraCharacter->GetAuraAbilitySystemComponent();
	for (const TSubclassOf ClassAbility : CommonAbilities)
	{	// Passives do not change according to levels (Eg: HitReact, ...)
		ASC->GiveAbility(FGameplayAbilitySpec(ClassAbility, 1));
	}
	for (const TSubclassOf Ability : GetClassDefaultInfo(AuraCharacter->CharacterClass)->ClassAbilities)
	{
		ASC->GiveAbility(FGameplayAbilitySpec(Ability, AuraCharacter->GetCharacterLevel()));
	}
}

void UCharacterClassDataAsset::SendXPToDeathCauser(AActor* Causer, const AAuraCharacterBase* DeadCharacter) const
{
	if (Causer == DeadCharacter) return;
	FGameplayEventData Payload;
	const FCharacterClassDefaultInfo* Info = GetClassDefaultInfo(DeadCharacter->CharacterClass);
	Payload.EventMagnitude = Info ? Info->XPReward.GetValueAtLevel(DeadCharacter->GetCharacterLevel()) : 0;
	if (Payload.EventMagnitude < UE_KINDA_SMALL_NUMBER) return;
	Payload.EventTag = AuraGameplayTags::Attributes_Meta_IncomingXP;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Causer, Payload.EventTag, Payload); // Last Hit player

	Payload.EventMagnitude *= .85f; // For allies
	for (const AAuraGameMode* GameMode = Cast<AAuraGameMode>(Causer->GetWorld()->GetAuthGameMode());
		const AAuraPlayerController* AuraController : GameMode->PlayerControllers)
	{
		if (AuraController->GetPawn() == Causer) continue;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AuraController->GetPawn(), Payload.EventTag, Payload);
	}
}
