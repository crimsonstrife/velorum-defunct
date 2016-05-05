// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "AdvKitTypes.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/Zones/AdvKitZoneLine.h"
#include "Environment/BuildModules/AdvKitBuildModule_Tightspace.h"

bool UAdvKitBuildModule_Tightspace::CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Cannot create transitions on non line zone object
	if (!ForZone->IsA(AAdvKitZoneLine::StaticClass()))
	{
		return false;
	}

	//Only valid for this movement mode
	if (ForZone->GetPhysics() != EAdvKitMovementMode::WalkingTightspace)
	{
		return false;
	}

	//Need to know character's movement properties to build transitions
	auto MovementProps = GetCharacterMovementProperties(ForCharacterClass);
	if (!MovementProps)
	{
		return false;
	}

	//No need to create transitions for a character that cannot use them
	return MovementProps->bCanWalkTightSpace;
}

void UAdvKitBuildModule_Tightspace::CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone)
{
	//Get character's attributes
	FVector CharacterHalfExtent = GetCharacterHalfExtent(ForCharacterClass, ForZone);
	float CharacterRadius = CharacterHalfExtent.X;
	float CharacterHalfHeight = CharacterHalfExtent.Z;

	//Get line configuration
	auto LineZone = Cast<AAdvKitZoneLine>(ForZone);
	check(LineZone);

	//Create start exit
	if (bCreateStartTransition)
	{
		FVector TransitionStart = LineZone->GetZoneStartWorld() + LineZone->GetActorRightVector() * CharacterRadius;
		FVector TransitionEnd = LineZone->GetZoneStartWorld() - LineZone->GetActorRightVector() * CharacterRadius;

		CreateTransitionPoint("ExitStart", LineZone, TransitionStart, TransitionEnd, ForCharacterClass, EAdvKitMovementMode::Walking);
	}

	//Create end exit
	if (bCreateEndTransition)
	{
		FVector TransitionStart = LineZone->GetZoneEndWorld() - LineZone->GetActorRightVector() * CharacterRadius;
		FVector TransitionEnd = LineZone->GetZoneEndWorld() + LineZone->GetActorRightVector() * CharacterRadius;
		
		CreateTransitionPoint("ExitEnd", LineZone, TransitionStart, TransitionEnd, ForCharacterClass, EAdvKitMovementMode::Walking);
	}
}
