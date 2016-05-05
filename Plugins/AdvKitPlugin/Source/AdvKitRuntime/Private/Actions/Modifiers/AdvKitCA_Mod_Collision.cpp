// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Modifiers/AdvKitCA_Mod_Collision.h"

UAdvKitCA_Mod_Collision::UAdvKitCA_Mod_Collision()
	: Super()
{
	bChangeCollision = true;
	bEnableCollision = false;
}


void UAdvKitCA_Mod_Collision::ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent)
{
	if(!ToCharacter)
	{
		return;
	}
	
	if (bChangeCollision)
	{
		ToCharacter->SetActorEnableCollision(bEnableCollision);
	}

	for (auto ResponseChange : ResponseChanges)
	{
		ToCharacter->GetCapsuleComponent()->SetCollisionResponseToChannel(ResponseChange.Channel, ResponseChange.Response);
	}
}
