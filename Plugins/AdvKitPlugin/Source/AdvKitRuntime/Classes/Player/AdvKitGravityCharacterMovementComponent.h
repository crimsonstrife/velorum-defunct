// Copyright 2015 Pascal Krabbe

#pragma once

#include "AdvKitCharacterMovementComponent.h"
#include "AdvKitOrientationComponent.h"
#include "AdvKitTypes.h"
#include "AdvKitGravityCharacterMovementComponent.generated.h"

class AAdvKitGravityCharacter;

/**
 * @brief Movement component that handles gravity set by the character's orientation component.
 */
UCLASS(Blueprintable, NotPlaceable)
class ADVKITRUNTIME_API UAdvKitGravityCharacterMovementComponent : public UAdvKitCharacterMovementComponent
{
	GENERATED_BODY()

public:

	/** Update the gravity orientation with the floor that is hit by the character's tracing component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bUpdateWorldOrientationByTracing;

	/** Update the gravity orientation with the floor the character is walking on */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bUpdateWorldOrientationByFloorResult;

	/** Reset gravity orientation when the character is falling (useful for things like magnetic boots) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	bool bResetOrientationWhenFalling;

protected:
	/** Reference to the owning gravity character */
	AAdvKitGravityCharacter* AdvKitGravityCharacterOwner;

	/** Flag if movement variables are currently transformed to local space */
	bool bMovementVariablesAreLocal;

public:
	/**
	 * Constructor
	 */
	UAdvKitGravityCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * Get the owning character's orientation component
	 * @return	Orientation component that handles gravity
	 */
	class UAdvKitOrientationComponent* GetOrientationComp() const;

	/**
	 * Checks if a hit result is a valid floor for wall walking
	 * @param	GravityDirection 	New direction of gravity
	 * @param	GravityOrigin		New origin of gravity
	 * @param	bForce				Set new gravity immediately, false to interpolate slowly
	 */
	UFUNCTION(BlueprintCallable, Category = "Gravity")
	virtual void SetGravityVector(const FVector& GravityDirection, const FVector& GravityOrigin = FVector::ZeroVector, bool bForce = false);

	/**
	 * Checks if a hit result is a valid floor for wall walking
	 * @param	NewFloor 	The floor to test
	 * @return	True if it is valid for wall walking
	 */
	virtual bool CanWallWalkOn(FFindFloorResult NewFloor);

	/**
	 * Updates orientation component based on configuration.
	 */
	virtual void UpdateWorldOrientation(float DeltaSeconds);
	
	UFUNCTION()
	virtual void OnOrientationChanged(const FTransform& OldOrientation);

	/**
	 * Utility that transforms all movement variables (Acceleration, Velocity) to local gravity space for easier calculation
	 */
	void TransformMovementVariablesWorldToLocal();

	/**
	 * Utility that transforms all movement variables (Acceleration, Velocity) from local gravity space to world space for easier calculation
	 */
	void TransformMovementVariablesLocalToWorld();

	//Begin UActorComponent Interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

	//Begin UMovementComponent Interface
	virtual FVector ConstrainDirectionToPlane(FVector Direction) const override;
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	//End UMovementComponent Interface

	//Begin CharacterMovementComponent Interface
	virtual void SimulateMovement(float DeltaSeconds) override;
	virtual void AdjustFloorHeight() override;
	virtual bool DoJump(bool bReplayingMoves) override;
	/*TODO Hotfix StartSwimming is not virtual */
	virtual void StartSwimmingHACK(FVector OldLocation, FVector OldVelocity, float timeTick, float remainingTime, int32 Iterations);
	virtual float ImmersionDepthGravity();// override; TODO was override of ImmersionDepth but virtual was removed in 4.9
	virtual void PerformAirControlForPathFollowing(FVector Direction, float ZDiff) override;
	virtual void StartFalling(int32 Iterations, float remainingTime, float timeTick, const FVector& Delta, const FVector& subLoc) override;
	void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;
	void SetFromSweepHACK(FFindFloorResult& Out, const FHitResult& InHit, const float InSweepFloorDist, const bool bIsWalkableFloor); //TODO FFindFloorResult does not export this function...
	void SetFromLineTraceHACK(FFindFloorResult& Out, const FHitResult& InHit, const float InSweepFloorDist, const float InLineDist, const bool bIsWalkableFloor); //TODO FFindFloorResult does not export this function...
	virtual bool IsWithinEdgeTolerance(const FVector& CapsuleLocation, const FVector& TestImpactPoint, const float CapsuleRadius) const;//TODO override; Base method is not virtual WHY?!
	virtual void ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance, float SweepDistance, FFindFloorResult& OutFloorResult, float SweepRadius, const FHitResult* DownwardSweepResult = NULL) const override;
	virtual void TwoWallAdjust(FVector &Delta, const FHitResult& Hit, const FVector &OldHitNormal) const override;
	virtual FVector HandleSlopeBoosting(const FVector& SlideResult, const FVector& Delta, const float Time, const FVector& Normal, const FHitResult& Hit) const override;
	virtual bool IsValidLandingSpot(const FVector& CapsuleLocation, const FHitResult& Hit) const override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void MaintainHorizontalGroundVelocity() override;
	virtual void PhysWalking(float DeltaSeconds, int32 Iterations) override;
	virtual void PhysFlying(float DeltaSeconds, int32 Iterations) override;
	virtual void PhysFalling(float DeltaSeconds, int32 Iterations) override;
	virtual void PhysSwimming(float DeltaSeconds, int32 Iterations) override;
	virtual void PhysicsRotation(float DeltaSeconds) override;
	virtual float SlideAlongSurface(const FVector& Delta, float Time, const FVector& InNormal, FHitResult &Hit, bool bHandleImpact) override;
	virtual bool IsWalkable(const FHitResult& Hit) const override;
	virtual void MoveSmooth(const FVector& InVelocity, const float DeltaSeconds, FStepDownResult* OutStepDownResult) override;
	virtual void UpdateFloorFromAdjustment() override;
	virtual FVector ComputeGroundMovementDelta(const FVector& Delta, const FHitResult& RampHit, const bool bHitFromLineTrace) const override;
	virtual void MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult = NULL) override;
	virtual FVector ConstrainInputAcceleration(const FVector& InputAcceleration) const override;
	//End CharacterMovementComponent Interface
};
