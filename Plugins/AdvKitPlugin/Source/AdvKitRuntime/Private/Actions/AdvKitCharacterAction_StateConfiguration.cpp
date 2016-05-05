// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "Actions/AdvKitCharacterAction_StateConfiguration.h"


UAdvKitCharacterAction_StateConfiguration::UAdvKitCharacterAction_StateConfiguration()
	: Super()
{
	CustomMovementMode = 0;
}

void UAdvKitCharacterAction_StateConfiguration::ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent)
{
	if (bChangeCollision)
	{
		ToCharacter->SetActorEnableCollision(bEnableCollision);
	}

	if (bChangeMovementMode)
	{
		ToMovementComponent->SetAdvMovementMode(MovementMode, CustomMovementMode);
	}

	if (bChangeYawRotationBind)
	{
		ToCharacter->bUseControllerRotationYawInFirstPerson = bUseControllerRotationYawInFirstPerson;
	}

	if (bChangeEquippedItem)
	{
		ToCharacter->InventoryManager->SetNewInventoryByClass(ItemClassToEquip, bInstantItemChange);
	}
}
