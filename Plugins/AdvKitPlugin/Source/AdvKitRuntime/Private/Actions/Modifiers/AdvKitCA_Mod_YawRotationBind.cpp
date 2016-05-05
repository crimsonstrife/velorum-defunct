// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Modifiers/AdvKitCA_Mod_YawRotationBind.h"


void UAdvKitCA_Mod_YawRotationBind::ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent)
{
	if(!ToCharacter)
	{
		return;
	}
	
	ToCharacter->bUseControllerRotationYawInFirstPerson = bUseControllerRotationYawInFirstPerson;
}
