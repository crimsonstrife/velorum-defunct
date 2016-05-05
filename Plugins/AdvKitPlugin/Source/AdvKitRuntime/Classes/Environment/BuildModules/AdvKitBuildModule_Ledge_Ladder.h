// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitTransitionBuilderModule.h"
#include "AdvKitBuildModule_Ledge_Ladder.generated.h"

class AAdvKitZone;
class AAdvKitCharacter;

/**
 * @brief Implementation of a transition builder module to create transitions between ledge and ladder zones
 */
UCLASS(Blueprintable, meta = (DisplayName = "Jumps between Ledge and Ladder"))
class ADVKITRUNTIME_API UAdvKitBuildModule_Ledge_Ladder : public UAdvKitTransitionBuilderModule
{
	GENERATED_BODY()

public:

	/** An offset to add when finding the closest location on the ladder */
	UPROPERTY(EditAnywhere)
	float LadderZOffset;

	/**
	 * Constructor
	 */
	UAdvKitBuildModule_Ledge_Ladder(const FObjectInitializer& ObjectInitializer);

	//Begin UAdvKitTransitionBuilderModule Interface
	virtual bool CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) override;

protected:
	virtual bool GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones) override;
	virtual bool CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone) override;
	//End UAdvKitTransitionBuilderModule Interface

	/**
	 * Creates transitions from a ledge zone to a ladder zone
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	Ledge 	Ledge to transition from
	 * @param	Ladder 	Ladder to transition to
	 * @return True if any transitions were created
	 */
	virtual bool CreateTransitionLedgeToLadder(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* Ledge, AAdvKitZone* Ladder);

	/**
	 * Creates transitions from a ladder zone to a ledge zone
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	Ladder 	Ladder to transition from
	 * @param	Ledge 	Ledge to transition to
	 * @return True if any transitions were created
	 */
	virtual bool CreateTransitionLadderToLedge(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* Ladder, AAdvKitZone* Ledge);
};
