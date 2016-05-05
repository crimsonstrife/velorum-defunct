// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Items/AdvKitUsable.h"


AAdvKitUsable::AAdvKitUsable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
}

bool AAdvKitUsable::CanBeUsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem)
{
	return true;
}

void AAdvKitUsable::UsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem)
{
	if (!CanBeUsedBy(Character, WithItem))
	{
		return;
	}

	if (OnUsedBy.IsBound())
	{
		OnUsedBy.Broadcast(Character, WithItem);
	}
}
