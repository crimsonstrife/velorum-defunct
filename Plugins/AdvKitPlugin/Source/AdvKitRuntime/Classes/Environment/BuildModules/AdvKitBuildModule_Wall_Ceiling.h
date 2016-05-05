// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitTransitionBuilderModule.h"
#include "AdvKitBuildModule_Wall_Ceiling.generated.h"

class AAdvKitZone;
class AAdvKitZoneRectangle;
class AAdvKitCharacter;

/**
 * @brief Implementation of a transition builder module to create transitions between wall and ceiling zones
 */
UCLASS(Blueprintable, meta = (DisplayName = "Transitions between Wall and Ceiling"))
class ADVKITRUNTIME_API UAdvKitBuildModule_Wall_Ceiling : public UAdvKitTransitionBuilderModule
{
	GENERATED_BODY()
public:
	//Begin UAdvKitTransitionBuilderModule Interface
	virtual bool CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) override;

protected:
	virtual bool GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones) override;
	virtual bool CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone) override;
	//End UAdvKitTransitionBuilderModule Interface

	/**
	 * Creates transitions from a wall zone to a ceiling zone
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	SourceWall 	Wall to transition from
	 * @param	TargetCeiling 	Ceiling to transition to
	 * @return True if any transitions were created
	 */
	virtual bool CreateTransitionFromWallToCeiling(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZoneRectangle* SourceWall, AAdvKitZoneRectangle* TargetCeiling);

	/**
	 * Creates transitions from a ceiling zone to a wall zone
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	SourceCeilnig Ceiling to transition from
	 * @param	TargetWall	Wall to transition to
	 * @return True if any transitions were created
	 */
	virtual bool CreateTransitionFromCeilingToWall(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZoneRectangle* SourceCeiling, AAdvKitZoneRectangle* TargetWall);
	
	/**
	 * Checks if a transition line defined by source and target aligns with the comparison direction
	 * @param	SourceLocation 	Start of the transition
	 * @param	TargetLocation 	End of the transition
	 * @param	ComparisonDirection 	Direction to compare to
	 * @return True if aligning
	 */
	virtual bool IsTransitionAligning(FVector SourceLocation, FVector TargetLocation, FVector ComparisonDirection);
};
