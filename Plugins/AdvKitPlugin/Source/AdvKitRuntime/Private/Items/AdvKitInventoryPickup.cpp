// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitInventoryManager.h"
#include "Items/AdvKitInventoryItem.h"
#include "Items/AdvKitInventoryPickup.h"

AAdvKitInventoryPickup::AAdvKitInventoryPickup(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bDestroyOnPickup = true;
	bRespawnAfterPickup = false;
}

void AAdvKitInventoryPickup::PickedUpBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem)
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (!IsValid(Character))
	{
		return;
	}

	UE_LOG(LogAdvKit, Log, TEXT("Picked up by"));

	if (!IsValid(Character->InventoryManager))
	{
		return;
	}

	UE_LOG(LogAdvKit, Log, TEXT("Character has inv manager"));

	FActorSpawnParameters Params;
	Params.Owner = Character->InventoryManager;
	Params.Instigator = Instigator;
	auto NewInventory = GetWorld()->SpawnActor<AAdvKitInventoryItem>(SpawnedInventoryClass, GetActorLocation(), GetActorRotation(),Params);
	if (!IsValid(NewInventory))
	{
		return;
	}

	NewInventory->SetReplicates(true);
	UE_LOG(LogAdvKit, Log, TEXT("Add"));
	Character->InventoryManager->AddInventory(NewInventory);

	Super::PickedUpBy_Implementation(Character, WithItem);
}
