// Copyright 2015 Pascal Krabbe

#pragma once

#include "Environment/AdvKitZone.h"
#include "AdvKitTypes.h"
#include "AdvKitZoneRectangle.generated.h"

/**
 * @brief Implementation of a zone that provides the character with a rectangle defined movement.
 */
UCLASS(Abstract)
class ADVKITRUNTIME_API AAdvKitZoneRectangle : public AAdvKitZone
{
	GENERATED_BODY()

public:
	/** "Lower Left" corner of the rectangle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rect", meta = (MakeEditWidget))
	FVector LocalRectMinBoundary;

	/** "Upper Right" corner of the rectangle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rect", meta = (MakeEditWidget))
	FVector LocalRectMaxBoundary;

public:
	/**
	 * Constructor
	 */
	AAdvKitZoneRectangle(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * Get the lower left corner of the rectangle in world space.
	 * @return	Lower left corner in world space.
	 */
	virtual FVector GetZoneRectMinWorld();

	/**
	 * Get the upper right corner of the rectangle in world space.
	 * @return	upper right corner in world space.
	 */
	virtual FVector GetZoneRectMaxWorld();

	//Begin AAdvKitZone Interface
	virtual FVector ConstrainPositionToZone_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator) override;
	virtual FVector ConstrainDirectionToZone_Implementation(const FVector& WorldDirection, const FVector& AtWorldLocation) override;
	virtual UAdvKitZoneLocation* GetClosestZoneLocation_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator) override;
	//End AAdvKitZone Interface

protected:
	/**
	 * Projects a given character extent onto the rectangle plane.
	 * @param	HalfExtent 	The extent of the character to project
	 * @param	ExtentWorldRotation 	World rotation of the character.
	 * @return	The projected half extent.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual FVector GetProjectedHalfExtent(const FVector& HalfExtent, const FRotator& ExtentWorldRotation = FRotator::ZeroRotator);
};
