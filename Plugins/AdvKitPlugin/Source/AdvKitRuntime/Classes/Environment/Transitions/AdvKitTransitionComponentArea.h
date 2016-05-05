// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitTransitionComponent.h"
#include "AdvKitTransitionComponentArea.generated.h"

/**
 * @brief Transition that is an area between two defined lines on the source and target zone. 
 */
UCLASS()
class ADVKITRUNTIME_API UAdvKitTransitionComponentArea : public UAdvKitTransitionComponent
{
	GENERATED_BODY()

	friend class FAdvKitTransitionComponentAreaVisualizer;

protected:
	/** Left start of the transition line(inside the source zone) */
	UPROPERTY(BlueprintReadonly, Category = "Transition")
	FBasedPosition MinSourcePosition;

	/** Left end of the transition line (inside of the target zone) */
	UPROPERTY(BlueprintReadonly, Category = "Transition")
	FBasedPosition MinTargetPosition;

	/** Right start of the transition (inside the source zone) */
	UPROPERTY(BlueprintReadonly, Category = "Transition")
	FBasedPosition MaxSourcePosition;

	/** Right end of the transition (inside of the target zone) */
	UPROPERTY(BlueprintReadonly, Category = "Transition")
	FBasedPosition MaxTargetPosition;

public:
	/**
	 * Initializes the transition
	 * @param	ForCharacter 	Character that can use the transition
	 * @param	MinWorldSource 	Left start of the transition line on the source zone in world space
	 * @param	MinWorldTarget 	Left end of the transition line on the target zone in world space
	 * @param	MaxWorldSource 	Right start of the transition line on the source zone in world space
	 * @param	MaxWorldTarget 	Right end of the transition line on the target zone in world space
	 * @param	NewTargetPhysics 	Movement mode the character will have after the transition
	 * @param	TargetZone 		Zone the character will be in after the transition
	 * @param	NewTargetCustomPhysics	Custom movement mode the character will have after the transition
	 */
	virtual void Init(TSubclassOf<AAdvKitCharacter> ForCharacter, const FVector& MinWorldSource, const FVector& MinWorldTarget, const FVector& MaxWorldSource, const FVector& MaxWorldTarget, EAdvKitMovementMode NewTargetPhysics, class AAdvKitZone* TargetZone = NULL, uint8 NewTargetCustomPhysics = 0);

	//Begin UActorComponent Interface
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	//End UActorComponent Interface

	//Begin UAdvKitTransitionComponent Interface
	virtual FVector GetClosestSourcePosition(const FVector& ToWorldPosition) override;
	virtual FVector GetClosestTargetPosition(const FVector& ToWorldPosition) override;
	//End UAdvKitTransitionComponent Interface

	/** @return The min source location of the transition in world space */
	FVector GetMinSourceLocation() const;

	/** @return The max source location of the transition in world space */
	FVector GetMaxSourceLocation() const;

	/** @return The min target location of the transition in world space */
	FVector GetMinTargetLocation() const;

	/** @return The max target location of the transition in world space */
	FVector GetMaxTargetLocation() const;

};
