// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitTransitionBuilderModule.h"
#include "AdvKitBuildModule_Ledge.generated.h"

class AAdvKitZone;
class AAdvKitCharacter;

/**
 * @brief Implementation of a transition builder module to create transitions for ledge zones
 */
UCLASS(Blueprintable, meta = (DisplayName = "Ledge Enter, Exit,  Corners and Jumps"))
class ADVKITRUNTIME_API UAdvKitBuildModule_Ledge : public UAdvKitTransitionBuilderModule
{
	GENERATED_BODY()

protected:
	/** Whether or not transitions to the side require jumps */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bSidewaysTransitionRequiresJump;

	/** Whether or not to create an exit transition to the platform above the ledge */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bCreateTransitionToPlatform;

	/** Whether or not to create an exit transition to the floor below the ledge */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	bool bCreateTransitionToFloor;

	/** The maximum trace distance down to find the floor when creating an exit transition */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (EditCondition = "bCreateTransitionToFloor"))
	float FloorTraceDistance;

public:
	/** Constructor */
	UAdvKitBuildModule_Ledge(const FObjectInitializer& ObjectInitializer);

protected:

	//Begin UAdvKitTransitionBuilderModule Interface
	virtual bool CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) override;
	virtual void CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) override;
	virtual bool GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones) override;
	virtual bool CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone) override;
	//End UAdvKitTransitionBuilderModule Interface

	/**
	 * Create a 45 degree inner or outer corner transition between two ledges.
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	SourceZone	Start of the transition
	 * @param	TargetZone 	End of the transition
	 * @return True if a transition was created
	 */
	virtual bool CreateCornerTransitionBetween(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone);

	/**
	 * Create a jump transition between two ledges.
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	SourceZone	Start of the transition
	 * @param	TargetZone 	End of the transition
	 * @return True if a transition was created
	 */
	virtual bool CreateJumpTransitionBetween(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone);
	
	/**
	 * Creates a horizontal jump transition between two ledges.
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	SourceZone	Start of the transition
	 * @param	TargetZone 	End of the transition
	 * @param	GlobalSoure	Start of the jump on the source zone
	 * @param	GlobalTarget	End of the jump on the target zone 	
	 * @return	True if a transition was created
	 */
	virtual bool CreateSideJumpTransitionBetween(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone, FVector GlobalSoure, FVector GlobalTarget);
	
	/**
	 * Create transitions to leave a zone
	 * @param	ForCharacterClass 	Character to create transitions for
	 * @param	ForZone 	Zone to create transitions for
	 * @return 
	 */
	virtual void CreateExitTransitions(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone);
};
