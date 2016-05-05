// Copyright 2015 Pascal Krabbe

#pragma once

#include "Actions/AdvKitCharacterAction_Arguments.h"
#include "AdvKitCA_ZoneTransition_Arguments.generated.h"

/**
 * @brief Arguments for a UAdvKitCA_ZoneTransition action. 
 * 
 * @see UAdvKitCA_ZoneTransition
 */
UCLASS(BlueprintType, Blueprintable, hidecategories = Object)
class UAdvKitCA_ZoneTransition_Arguments : public UAdvKitCharacterAction_Arguments
{
	GENERATED_BODY()
public:
	/** Transition component along which the transition happens. */
	UPROPERTY(BlueprintReadWrite)
	UAdvKitTransitionComponent* Transition;

	/** Target zone of the transition */
	UPROPERTY(BlueprintReadWrite)
	class AAdvKitZone* Zone;
};
