// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "Actions/AdvKitCharacterActionSingleAnimation.h"
#include "Player/AdvKitCharacterMovementComponent.h"
#include "AdvKitCA_PhysicsTransition.generated.h"

/** @brief Helper struct since UE4 does not support TMap properties. This handles configuring the transition
 * mapping to set which actions take care of which physics mode transition. 
 */
USTRUCT()
struct FAdvKitPhysicsTransition
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
	/** Mode to transition from */
	EAdvKitMovementMode FromMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
	/** Mode to transition to */
	EAdvKitMovementMode ToMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
	/** Action to take when going FromMode -> ToMode*/
	TSubclassOf<UAdvKitCharacterAction> TransitionAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
	/** Action to take when going ToMode -> FromMode */
	TSubclassOf<UAdvKitCharacterAction> ReverseAction;
};

/**
 * @brief This action is responsible for redirecting to sub actions that take care of movement mode
 * changes.
 */
UCLASS(abstract, hidecategories = Action)
class ADVKITRUNTIME_API UAdvKitCA_PhysicsTransition : public UAdvKitCharacterAction
{
	GENERATED_BODY()

public:
	/**
	 * Constructor
	 */
	UAdvKitCA_PhysicsTransition(const FObjectInitializer& ObjectInitializer);

protected:

	/** Flag to enable debug output for transitions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
	bool bLogDebug;

	/** Configuration map for the transition action */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Transitions")
	TArray<FAdvKitPhysicsTransition> DefaultTransitionActions;

	//TODO replace uint8 with EAdvKitMovementMode (currently unsupported (4.9))
	/** Actual transition map, configured based upon the configured DefaultTranstionActions array */
	TMap < uint8, TMap<uint8, TSubclassOf<UAdvKitCharacterAction> > > TransitionMap;
	
	
public:
	//Begin UActorComponent Interface
	virtual void InitializeComponent() override;
	//End UActorComponent Interface

	/** Begin UAdvKitCharacterAction Interface */
	virtual FAdvKitActionResponse Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments = NULL, UAdvKitCharacterAction* InterruptedOther = NULL) override;
	virtual FAdvKitActionResponse Stop_Implementation(const UAdvKitCharacterAction* InterruptedBy = NULL) override;
	virtual bool CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const override;
	virtual TArray<TSubclassOf<UAdvKitCharacterAction>> GetAdditionalRequiredActions_Implementation() override;
	/** End UAdvKitCharacterAction Interface */
	
	/**
	 * Create arguments for the physics transition action
	 * @param	From 	Movement mode to transition from
	 * @param	To 	Movement mode to transition to
	 * @param	SubArguments 	Arguments to supply to the chosen redirection action
	 * @return Arguments object containing given parameters
	 */
	static class UAdvKitCharacterAction_Arguments* MakeArguments(EAdvKitMovementMode From, EAdvKitMovementMode To, UAdvKitCharacterAction_Arguments* SubArguments = NULL);

protected:

	/**
	 * Adds a new transition to the transitions map
	 * @param	FromMode 	Movement mode to transition from
	 * @param	ToMode	Movement mode to transition to
	 * @param	TransitionAction 	Action to take care of the transition
	 */
	virtual void AddTransiton(EAdvKitMovementMode FromMode, EAdvKitMovementMode ToMode, TSubclassOf<UAdvKitCharacterAction> TransitionAction);
};
