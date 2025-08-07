// Copyright Hung


#include "UI/Data/MessageInfo.h"

namespace MessageTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message,"Message", "Parent Tag")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthCrystal,"Message.HealthCrystal", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_HealthPotion,"Message.HealthPotion", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaCrystal,"Message.ManaCrystal", "")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Message_ManaPotion,"Message.ManaPotion", "")
}

FAuraMessageInfo UMessageInfo::FindMessageInfoForTag(const FGameplayTag& MessageTag) const
{
	//TODO: return ptr
	return MessageInformation.FindRef(MessageTag);
}
