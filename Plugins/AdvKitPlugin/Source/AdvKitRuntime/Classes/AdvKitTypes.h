// Copyright 2015 Pascal Krabbe

#pragma once

#include "AdvKitTypes.generated.h"

/**
 * @brief Enumeration for movement mode of the @see UAdvKitMovementComponent
 */
UENUM(BlueprintType)
enum class EAdvKitMovementMode : uint8
{
	None				UMETA(DisplayName = "None"),
	Walking				UMETA(DisplayName = "Walking"),
	NavWalking			UMETA(DisplayName = "Navmesh Walking"),
	Falling				UMETA(DisplayName = "Falling"),
	Swimming			UMETA(DisplayName = "Swimming"),
	Flying				UMETA(DisplayName = "Flying"),
	//Enums above to preserve casting conversion from MovementMode to AdvMovementMode
	ClimbingWall		UMETA(DisplayName = "Wall Climbing"),
	ClimbingCeiling		UMETA(DisplayName = "Ceiling Climbing"),
	ClimbingLedge		UMETA(DisplayName = "Ledge Climbing"),
	ClimbingLadder		UMETA(DisplayName = "Ladder Climbing"),
	WalkingTightspace	UMETA(DisplayName = "Tight Space Walking"),
	WalkingBalance		UMETA(DisplayName = "Balance Walking"),
	ActionDriven		UMETA(DisplayName = "Action Driven"),
	Custom				UMETA(DisplayName = "Custom"),
};

#if WITH_EDITOR

/**
 * @brief Convert enum value of movement mode to a string
 *
 * @param	EnumValue 	The enum value to convert
 * @return Name of the mode
 */
FString GetMovementModeDisplayName(EAdvKitMovementMode EnumValue);
#endif 
