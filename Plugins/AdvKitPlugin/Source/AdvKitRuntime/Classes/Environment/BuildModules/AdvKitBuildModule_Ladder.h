// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitTransitionBuilderModule.h"
#include "AdvKitBuildModule_Ladder.generated.h"

class AAdvKitZone;
class AAdvKitCharacter;

/**
 * @brief Implementation of a transition builder module to create transitions for ladder zones
 */
UCLASS(Blueprintable, meta = (DisplayName = "Ladder Enter and Exit"))
class ADVKITRUNTIME_API UAdvKitBuildModule_Ladder : public UAdvKitTransitionBuilderModule
{
	GENERATED_BODY()

protected:
	/** Whether or not transitions to the side require jumps */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bSidewaysTransitionRequiresJump;

	/** Whether or not to create an exit transition at the top of the ladder */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bCreateTransitionToPlatform;

	/** Whether or not to create an exit transition at bottom of the ladder */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bCreateTransitionToFloor;

	//Begin UAdvKitTransitionBuilderModule Interface
	virtual bool CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) override;
	virtual void CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) override;
	virtual bool GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones) override;
	virtual bool CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone) override;
	//End UAdvKitTransitionBuilderModule Interface

	/**
	 * Create transitions to leave a zone
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	ForZone 	Zone to create transitions for
	 * @return 
	 */
	virtual void CreateExitTransitions(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone);
};
