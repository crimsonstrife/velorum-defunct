// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "AdvKitZoneLocation.generated.h"

/**
 * @brief Base class for object that defines a location inside a zone. 
 * 
 * Different zones can have different location specifiers. This class 
 * takes care of storing a location and all the conversion methods 
 * required to position the character.
 */
UCLASS()
class ADVKITRUNTIME_API UAdvKitZoneLocation : public UObject
{
	GENERATED_BODY()

public:

	/** Zone this location is in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit", Replicated)
	class AAdvKitZone* Zone;

	/** Relative location to the zone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit", Replicated)
	FVector LocalPosition;

public:
	/**
	 * Set all relevant values for the location
	 * @param	InZone 	Zone this location is in
	 * @param	InPosition 	Global position this location holds
	 * @return 
	 */
	virtual void Set(class AAdvKitZone* InZone, const FVector& InPosition);

	/**
	 * Queries the global position of this location
	 * @return The position in world space
	 */
	virtual FVector GetGlobalPosition() const;

	/**
	 * Constrains a direction in world space to align with the zone.
	 * @param	WorldDirection	Direction to constrain.
	 * @return	The restricted direction.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector ConstrainDirection(const FVector& WorldDirection);
	
	/**
	 * Attempts to move the location inside the zone.
	 * @param	WorldDesiredDelta	Direction to move in.
	 * @param	HalfExtent 	Half size of the character, describing its capsule (e.g. capsule radius and half height)
	 * @param	ExtentWorldRotation 	Rotation of the character.
	 * @return	True if the loation could be moved, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool MoveInDirection(const FVector& WorldDesiredDelta, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);
};
