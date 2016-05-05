// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitTransitionComponent.h"
#include "AdvKitTransitionComponentPoint.generated.h"

/**
 * @brief Transition that is a straight line defined by a start and end point. 
 */
UCLASS()
class ADVKITRUNTIME_API UAdvKitTransitionComponentPoint : public UAdvKitTransitionComponent
{
	GENERATED_BODY()
public:
	/** Based position on the target zone */
	UPROPERTY(BlueprintReadonly, Category = "Transition")
	FBasedPosition TargetPosition;

	/** Based position on the start zone */
	UPROPERTY(BlueprintReadonly, Category = "Transition")
	FBasedPosition SourcePosition;

public:

	/**
	 * Initializes the transition
	 * @param	ForCharacter 	Character that can use the transition
	 * @param	WorldSource 	Start of the transition line on the source zone in world space
	 * @param	WorldTarget 	End of the transition line on the target zone in world space
	 * @param	NewTargetPhysics 	Movement mode the character will have after the transition
	 * @param	TargetZone 		Zone the character will be in after the transition
	 * @param	NewTargetCustomPhysics	Custom movement mode the character will have after the transition
	 */
	virtual void Init(TSubclassOf<AAdvKitCharacter> ForCharacter, const FVector& WorldSource, const FVector& WorldTarget, EAdvKitMovementMode NewTargetPhysics, class AAdvKitZone* TargetZone = NULL, uint8 NewTargetCustomPhysics = 0);

	//Begin UActorComponent Interface
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
	//End UActorComponent Interface

	//Begin UAdvKitTransitionComponent Interface
	virtual FVector GetClosestSourcePosition(const FVector& ToWorldPosition) override;
	virtual FVector GetClosestTargetPosition(const FVector& ToWorldPosition) override;
	//End UAdvKitTransitionComponent Interface

	/** @return The source location of the transition in world space */
	FVector GetSourceLocation() const;

	/** @return The target location of the transition in world space */
	FVector GetTargetLocation() const;
};
