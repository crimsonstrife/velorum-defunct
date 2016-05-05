// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Modifiers/AdvKitCA_Mod_MovementMode.h"


void UAdvKitCA_Mod_MovementMode::ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent)
{
	if(!ToMovementComponent)
	{
		return;
	}
	
	ToMovementComponent->SetAdvMovementMode(MovementMode, CustomMovementMode);
}
