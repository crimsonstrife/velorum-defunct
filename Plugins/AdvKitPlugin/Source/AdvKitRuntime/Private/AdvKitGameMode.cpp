// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "Player/AdvKitPlayerController.h"
#include "Player/AdvKitCharacter.h"

#include "AdvKitGameMode.h"


AAdvKitGameMode::AAdvKitGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Set default classes to AdvKit classes.
	PlayerControllerClass = AAdvKitPlayerController::StaticClass();
	DefaultPawnClass = AAdvKitCharacter::StaticClass();
}
