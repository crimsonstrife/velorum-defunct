// Copyright 2015 Pascal Krabbe

#pragma once

#include "Components/ActorComponent.h"
#include "AdvKitTypes.h"
#include "AdvKitTransitionComponent.generated.h"

class AAdvKitCharacter;
class UAdvKitTransitionBuilderModule;

/**
 * @brief Base class for transitions between zones.
 */
UCLASS(abstract, HideCategories=(Component, Activation))
class ADVKITRUNTIME_API UAdvKitTransitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Class of build module responsible for creating this transitions, used to update */
	UPROPERTY()
	UAdvKitTransitionBuilderModule* CreatedByBuildModule;

	/** Identifier given by the build module responsible for creating this transitions, used to update */
	UPROPERTY()
	FString TransitionId;

	/** Flag that indicates this transition is dynamic. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	bool bIsDynamic;

	/** Flag that indicates this transition is dynamic. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Transition")
	bool bDynamicTransitionActive;

	/** Character needs to jump */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	uint8 bNeedsJump : 1;

	/** Character needs to go this direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	FVector TransitionDirection;

	/** Character's direction can differ this much from the required direction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float MinDirectionNormal;

	/** Character's location offset from the transition can differ this much from the closest transition source. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition")
	float MinLocationRadius;

	/** Which kind of character can use the transition */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Transition")
	TSubclassOf<AAdvKitCharacter> UseableBy;

	/** Target of the transition */
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, /*DuplicateTransient, */Category = "Transition")
	TWeakObjectPtr<class AAdvKitZone> TargetZone;

	/** Movement mode the character will have after this transition */
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Transition")
	EAdvKitMovementMode TargetPhysics;

	/** Custom movement mode the character will have after this transition */
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Transition")
	uint8 TargetCustomPhysics;

	/**
	 * Query to get the closest location where a transition can start in relation to another location.
	 * @param	ToWorldPosition 	Location in world space to find reference point to.
	 * @return Closest location in world space to start the transition.
	 */
	UFUNCTION(BlueprintCallable, Category="Transition")
	virtual FVector GetClosestSourcePosition(const FVector& ToWorldPosition);

	/**
	 * Query to get the closest location where a transition will end in relation to another location.
	 * @param	ToWorldPosition 	Location in world space to find reference point to.
	 * @return Closest location in world space to end the transition.
	 */
	UFUNCTION(BlueprintCallable, Category="Transition")
	virtual FVector GetClosestTargetPosition(const FVector& ToWorldPosition);

	/**
	 * Check if the transition can be used.
	 * @param	Character 	Character that wants to transition
	 * @param	AtLocation 	World location from where to start the transition if possible
	 * @param	InDirection	Direction in world space where to transition to
	 * @param	bIsJump	Whether or not the transition is done with a jump
	 * @return True if transition can be used.
	 */
	UFUNCTION(BlueprintCallable, Category="Transition")
	virtual bool CanTransition(AAdvKitCharacter* Character, const FVector& AtLocation, const FVector& InDirection, bool bIsJump);

	/**
	 * Dynamic transitions can be invalid (e.g. a transition point is too far off) and not be 
	 * used during this time (think moving platforms).
	 * @return True if the dynamic transition can be used.
	 */
	UFUNCTION(BlueprintCallable, Category="Transition")
	virtual bool IsDynamicTransitionValid();

public:
	/**
	 * Constructor
	 */
	UAdvKitTransitionComponent(const FObjectInitializer& ObjectInitializer);
};
