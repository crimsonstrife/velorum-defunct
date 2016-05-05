// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Modifiers/AdvKitCA_Mod_Item.h"


void UAdvKitCA_Mod_Item::ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent)
{
	if(!ToCharacter)
	{
		return;
	}
	
	if(!ToCharacter->InventoryManager)
	{
		return;
	}
	
	ToCharacter->InventoryManager->SetNewInventoryByClass(ItemClassToEquip, bInstantItemChange);
}
