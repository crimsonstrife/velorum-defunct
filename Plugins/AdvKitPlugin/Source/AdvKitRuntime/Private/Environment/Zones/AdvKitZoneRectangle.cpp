// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Environment/Zones/AdvKitZoneRectangle.h"

AAdvKitZoneRectangle::AAdvKitZoneRectangle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LocalRectMinBoundary = FVector(20, -100, -100);
	LocalRectMaxBoundary = FVector(20, 100, 100);
}

FVector AAdvKitZoneRectangle::GetZoneRectMinWorld()
{
	return GetTransform().TransformPositionNoScale(LocalRectMinBoundary);
}

FVector AAdvKitZoneRectangle::GetZoneRectMaxWorld()
{
	return GetTransform().TransformPositionNoScale(LocalRectMaxBoundary);
}

FVector AAdvKitZoneRectangle::ConstrainPositionToZone_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	//Convert location to local space
	FVector PositionLocal = GetTransform().InverseTransformPositionNoScale(WorldPosition);

	//Get projected half extent
	FVector ProjectedExtent = GetProjectedHalfExtent(HalfExtent,ExtentWorldRotation);

	//Constrain location to plane and clamp sides
	PositionLocal.X = LocalRectMinBoundary.X;
	PositionLocal.Y = FMath::Clamp<float>(PositionLocal.Y, LocalRectMinBoundary.Y + ProjectedExtent.Y, LocalRectMaxBoundary.Y - ProjectedExtent.Y);
	PositionLocal.Z = FMath::Clamp<float>(PositionLocal.Z, LocalRectMinBoundary.Z + ProjectedExtent.Z, LocalRectMaxBoundary.Z - ProjectedExtent.Z);

	//Transform result back to world space
	return GetTransform().TransformPositionNoScale(PositionLocal);
}

FVector AAdvKitZoneRectangle::ConstrainDirectionToZone_Implementation(const FVector& WorldDirection, const FVector& AtWorldLocation)
{
	//Transform direction to local space
	FVector DirectionLocal = GetTransform().InverseTransformVectorNoScale(WorldDirection);

	//Project onto plane
	DirectionLocal.X = 0;

	//Transform back to world space
	return GetTransform().TransformVectorNoScale(DirectionLocal);
}


class UAdvKitZoneLocation* AAdvKitZoneRectangle::GetClosestZoneLocation_Implementation(const FVector& WorldPosition, const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	//Create new location
	UAdvKitZoneLocation* NewLocation = NewObject<UAdvKitZoneLocation>(this);

	//Initialize
	NewLocation->Set(this, ConstrainPositionToZone(WorldPosition, HalfExtent, ExtentWorldRotation));
	return NewLocation;
}


FVector AAdvKitZoneRectangle::GetProjectedHalfExtent(const FVector& HalfExtent, const FRotator& ExtentWorldRotation)
{
	//Transform to local space
	FVector GlobalExtent = ExtentWorldRotation.RotateVector(HalfExtent);
	FVector LocalHalfExtent = GetActorQuat().Inverse() * GlobalExtent;
	
	//Make sure values are positive
	LocalHalfExtent.X = FMath::Abs<float>(LocalHalfExtent.X);
	LocalHalfExtent.Y = FMath::Abs<float>(LocalHalfExtent.Y);
	LocalHalfExtent.Z = FMath::Abs<float>(LocalHalfExtent.Z);
	return LocalHalfExtent;
}
