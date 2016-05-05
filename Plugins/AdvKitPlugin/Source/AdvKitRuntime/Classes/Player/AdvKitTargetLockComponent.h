#pragma once

#include "AdvKitTargetLockComponent.generated.h"

class AAdvKitCharacter;

/**
 * @brief This component tries to keep an attached camera between the parent character and its target so 
 * that both are in view at all times.
 */
UCLASS(ClassGroup = Camera, meta = (BlueprintSpawnableComponent), hideCategories = (Mobility))
class ADVKITRUNTIME_API UAdvKitTargetLockComponent : public USceneComponent
{
	GENERATED_UCLASS_BODY()

	/** The minimal distance the camera is away from the center point between character and target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MinimumDistanceFromCenter;

	/** Offset to add to the center point */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	FVector TargetOffset;

	// Begin UActorComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// End of UActorComponent interface

	// Begin USceneComponent interface
	virtual bool HasAnySockets() const override;
	virtual FTransform GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace = RTS_World) const override;
	virtual void QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const override;
	// End of USceneComponent interface

	/** The name of the socket to attach the camera to */
	static const FName SocketName;

protected:
	/** Current distance from the center */
	float DistanceFromCenter;

	/** In order to calculate the socket location the rotation of the view needs to be cached */
	FQuat CachedViewRotation;


	/**
	 * Retrieve the target point of the owning character
	 *
	 * @return Target component if any
	 */
	virtual AAdvKitTargetPoint* GetTarget();

	/**
	 * Retrieve a cast pointer to the owning character
	 *
	 * @return Owning character if any
 	 */
	virtual AAdvKitCharacter* GetOwningCharacter();
};
