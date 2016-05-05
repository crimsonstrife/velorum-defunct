// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitTransitionBuilderModule.h"
#include "AdvKitBuildModule_Wall.generated.h"

class AAdvKitZone;
class AAdvKitCharacter;

/**
 * @brief Implementation of a transition builder module to create transitions for wall zones
 */
UCLASS(Blueprintable, meta = (DisplayName = "Wall Enter, Exit and Corners"))
class ADVKITRUNTIME_API UAdvKitBuildModule_Wall : public UAdvKitTransitionBuilderModule
{
	GENERATED_BODY()

protected:
	/** Whether or not to create an exit transition to the platform above the wall */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bCreateTransitionToPlatform;

	/** Whether or not to create an exit transition to the floor below the wall */
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
