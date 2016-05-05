// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Modifiers/AdvKitCA_Mod_Auto_MovementMode.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_Arguments.h"
#include "Environment/AdvKitZone.h"
#include "Environment/AdvKitTransitionComponent.h"


void UAdvKitCA_Mod_Auto_MovementMode::ApplyToCharacter(class AAdvKitCharacter* ToCharacter, class UAdvKitCharacterMovementComponent* ToMovementComponent)
{
	auto OwnerAction = GetTypedOuter<UAdvKitCA_ZoneTransition>();
	if (!OwnerAction)
	{
		OwnerAction = Cast<UAdvKitCA_ZoneTransition>(ToCharacter->GetActiveAction());
	}

	if (!OwnerAction)
	{
		return;
	}

	auto Arguments = OwnerAction->GetCurrentArguments();
	if (!Arguments)
	{
		return;
	}

	auto Zone = Arguments->Zone;
	if (Zone)
	{
		ToMovementComponent->SetAdvMovementMode(Zone->GetPhysics(), Zone->GetCustomPhysics());
	}

	auto Transition = Arguments->Transition;
	if (Transition)
	{
		ToMovementComponent->SetAdvMovementMode(Transition->TargetPhysics, Transition->TargetCustomPhysics);
	}
}
