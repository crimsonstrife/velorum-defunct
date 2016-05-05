// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitZone.h"
#include "AdvKitTypes.h"
#include "AdvKitZoneLine.generated.h"

/**
 * @brief Implementation of a zone that consists of a straight line defined by a beginning and end point.
 * The character can only move in two directions along the line.
 */
UCLASS(Abstract)
class ADVKITRUNTIME_API AAdvKitZoneLine : public AAdvKitZone
{
	GENERATED_BODY()

public:
	/** Start of the line */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone", meta = (MakeEditWidget))
	FVector ZoneStart;

	/** Whether or not the start of the line is clamped by the character's extent (think it as being a wall). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	bool bConstrainExtentAtStart;

	/** End of the line */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone", meta = (MakeEditWidget))
	FVector ZoneEnd;

	/** Whether or not the end of the line is clamped by the character's extent (think it as being a wall). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	bool bConstrainExtentAtEnd;

public:
	/**
	 * Constructor
	 */
	AAdvKitZoneLine(const FObjectInitializer& ObjectInitializer);

	/**
	 * Get the start of the line in world space.
	 * @return	Start of the line in world space.
	 */
	virtual FVector GetZoneStartWorld();

	/**
	 * Get the end of the line in world space.
	 * @return	End of the line in world space.
	 */
	virtual FVector GetZoneEndWorld();

	//Begin AdvKitZone Interface
	virtual FVector ConstrainPositionToZone_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator) override;
	virtual FVector ConstrainDirectionToZone_Implementation(const FVector& WorldDirection, const FVector& AtWorldLocation) override;
	virtual UAdvKitZoneLocation* GetClosestZoneLocation_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator) override;
	//End AdvKitZone Interface

protected:

	/**
	 * Projects a given character extent onto a line defined by direction.
	 * @param	LocalDirection 	The line's direction to project onto in local space
	 * @param	HalfExtent 	The extent of the character to project
	 * @param	ExtentWorldRotation 	World rotation of the character.
	 * @return	The projected half extent.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetProjectedHalfExtent(const FVector &LocalDirection, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);
};
