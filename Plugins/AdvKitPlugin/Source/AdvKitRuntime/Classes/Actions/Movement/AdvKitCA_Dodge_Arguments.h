// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/AdvKitCharacterAction_Arguments.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_Dodge_Arguments.generated.h"

/**
 * @brief Arguments for the @see UAdvKitCA_Dodge action.
 */
UCLASS(BlueprintType, Blueprintable, hidecategories = Object)
class UAdvKitCA_Dodge_Arguments : public UAdvKitCharacterAction_Arguments
{
	GENERATED_BODY()
public:
	/** Direction to dodge towards */
	UPROPERTY(BlueprintReadWrite)
	FVector DodgeDirection;
};
