// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCharacterActionPhysics.h"
#include "Actions/Modifiers/AdvKitCA_Mod_MovementMode.h"

UAdvKitCharacterActionPhysics::UAdvKitCharacterActionPhysics(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	auto MovementModeModStart = ObjectInitializer.CreateDefaultSubobject<UAdvKitCA_Mod_MovementMode>(this, "MovementModeModStart");
	MovementModeModStart->MovementMode = EAdvKitMovementMode::ActionDriven;
	StartModifiers.Add(MovementModeModStart);

	auto MovementModeModStop = ObjectInitializer.CreateDefaultSubobject<UAdvKitCA_Mod_MovementMode>(this, "MovementModeModStop");
	MovementModeModStop->MovementMode = EAdvKitMovementMode::ActionDriven;
	StopModifiers.Add(MovementModeModStop);
}

void UAdvKitCharacterActionPhysics::ExecutePhysicsMovement(float DeltaSeconds/*, int32 Iterations*/)
{

}

void UAdvKitCharacterActionPhysics::ExecutePhysicsRotation(float DeltaSeconds)
{

}