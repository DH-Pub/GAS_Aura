// Copyright Hung


#include "UI/Data/MessageInfo.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthCrystal,"Message.HealthCrystal", "")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthPotion,"Message.HealthPotion", "")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaCrystal,"Message.ManaCrystal", "")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaPotion,"Message.ManaPotion", "")

FAuraMessageInfo UMessageInfo::FindMessageInfoForTag(const FGameplayTag& MessageTag) const
{
	return MessageInformation.FindRef(MessageTag);
}
