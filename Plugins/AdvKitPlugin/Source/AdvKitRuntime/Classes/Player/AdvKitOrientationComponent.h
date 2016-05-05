// Copyright 2015 Pascal Krabbe

#pragma once

#include "Components/SceneComponent.h"
#include "AdvKitOrientationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOrientationChanged, const FTransform&, OldOrientation);

/**
 * @brief Component to provide transformation coordinate methods.
 * 
 * One of the core classes of the arbitrary gravity system. This component's transform provides
 * a separate coordinate system aligning with the custom gravity vector as Z. It has several methods 
 * to easily convert vectors and rotators between world and local space.
 */
UCLASS(ClassGroup = Utility, meta = (BlueprintSpawnableComponent))
class ADVKITRUNTIME_API UAdvKitOrientationComponent : public USceneComponent
{
	GENERATED_BODY()

protected:
	/** Setting a new orientation location will not immediately change it, but rather interpolate 
	  * to it using this value multiplied with DeltaTime
	  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LocationLerpSpeed;

	/** Setting a new orientation will not immediately change it, but rather interpolate 
	  * to it using this value multiplied with DeltaTime
	  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationLerpSpeed;

	/** Transform the component should interpolate to */
	FTransform TargetTransform;

public:

	/** Callback when the orientation of this component changes */
	UPROPERTY(BlueprintAssignable, Category="AdvKit")
	FOrientationChanged OnOrientationChanged;

	/**
	 * Constructor
	 */
	UAdvKitOrientationComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * Transforms given direction from world space to local space
	 * @param	Direction 	World direction
	 * @return Direction transformed to local space
	 */
	FVector TransformDirectionW2L(const FVector& Direction);

	/**
	 * Transforms given direction from local space to world space
	 * @param	Direction 	Local direction
	 * @return Direction transformed to world space
	 */
	FVector TransformDirectionL2W(const FVector& Direction);

	/**
	 * Transforms given position from world space to local space
	 * @param	Position	World position
	 * @return Position transformed to local space
	 */
	FVector TransformPositionW2L(const FVector& Position);

	/**
	 * Transforms given position from local space to world space
	 * @param	Position	Local position
	 * @return	Position transformed to world space
	 */
	FVector TransformPositionL2W(const FVector& Position);

	/**
	 * Transforms given rotation from world space to local space
	 * @param	Rotation World rotation
	 * @return	Rotation transformed to local space
	 */
	FRotator TransformRotationW2L(const FRotator& Rotation);

	/**
	 * Transforms given rotation from local space to world space
	 * @param	Rotation Local rotation
	 * @return	Rotation transformed to world space
	 */
	FRotator TransformRotationL2W(const FRotator& Rotation);

	/**
	 * Transforms given rotation from world space to local space
	 * @param	Rotation World rotation
	 * @return	Rotation transformed to local space
	 */
	FQuat TransformRotationW2L(const FQuat& Rotation);

	/**
	 * Transforms given rotation from local space to world space
	 * @param	Rotation Local rotation
	 * @return	Rotation transformed to world space
	 */
	FQuat TransformRotationL2W(const FQuat& Rotation);

	/**
	 * Sets a new orientation from an origin location and normal in 
	 * world space. Input is interpolated with current transform, using
	 * Location- and RotationLerpSpeed.
	 *
	 * @param	Origin		Center of the new orientation
	 * @param	UpNormal	Up vector of the new orientation (in world space)
	 * @param	bForce		Ignore interpolation, hard set values
	 */
	UFUNCTION(BlueprintCallable, Category="AdvKit")
	void SetOrientation(const FVector Origin, const FVector UpNormal, bool bForce = false);

	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//End UActorComponent Interface

protected:
	/**
	 * Sets a new transform for this component and executes orientation callback
	 * @param	NewTransform 	New transform to set
	 */
	void UpdateTransform(const FTransform& NewTransform);
};
