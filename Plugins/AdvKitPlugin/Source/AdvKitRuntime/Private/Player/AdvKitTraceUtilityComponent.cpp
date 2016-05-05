// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Player/AdvKitTraceUtilityComponent.h"

UAdvKitTraceUtilityComponent::UAdvKitTraceUtilityComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TraceLength = 200;
	DebugLineColor = FColor::Red;
	DebugLineThickness = 0;
	bHiddenInGame = true;
}


void UAdvKitTraceUtilityComponent::DrawDebug()
{
	//Values for line
	const FVector Forward = GetForwardVector();
	const FVector Right = GetRightVector();
	const FVector LineStart = GetStart();
	const FVector LineEnd = GetEnd();
	const float	ArrowLength = FMath::Min<float>(16.0f, TraceLength*0.25f);

	//Draw lines and arrow head
	DrawDebugLine(GetWorld(), LineStart, LineEnd, DebugLineColor, false, -1, 0, DebugLineThickness);
	DrawDebugLine(GetWorld(), LineEnd, LineEnd + (-Forward + Right).GetSafeNormal()*ArrowLength, DebugLineColor, false, -1, 0, DebugLineThickness);
	DrawDebugLine(GetWorld(), LineEnd, LineEnd + (-Forward - Right).GetSafeNormal()*ArrowLength, DebugLineColor, false, -1, 0, DebugLineThickness);
}

void UAdvKitTraceUtilityComponent::SetTraceLength(float NewLength)
{
	TraceLength = NewLength;
}

float UAdvKitTraceUtilityComponent::GetTraceLength() const
{
	return TraceLength;
}

void UAdvKitTraceUtilityComponent::SetDebugLineColor(FColor NewColor)
{
	DebugLineColor = NewColor;
}

FColor UAdvKitTraceUtilityComponent::GetDebugLineColor() const
{
	return DebugLineColor;
}

void UAdvKitTraceUtilityComponent::SetDebugLineThickness(float NewThickness)
{
	DebugLineThickness = NewThickness;
}

float UAdvKitTraceUtilityComponent::GetDebugLineThickness() const
{
	return DebugLineThickness;
}

FVector UAdvKitTraceUtilityComponent::GetStart()
{
	//This is necessary because MoveUpdatedComponent does not automatically call UpdateChildTransforms 
	// so this component's transform might be out of date when trace is called
	USceneComponent* Root = GetAttachmentRoot();
	if (Root)
	{
		Root->UpdateChildTransforms();
	}

	return GetComponentLocation();
}

FVector UAdvKitTraceUtilityComponent::GetEnd()
{
	//This is necessary because MoveUpdatedComponent does not automatically call UpdateChildTransforms 
	// so this component's transform might be out of date when trace is called
	USceneComponent* Root = GetAttachmentRoot();
	if (Root)
	{
		Root->UpdateChildTransforms();
	}

	return GetStart() + GetComponentQuat().RotateVector(FVector::ForwardVector)*TraceLength;
}

bool UAdvKitTraceUtilityComponent::LineTraceTest(ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam)
{
	return GetWorld()->LineTraceTestByChannel(GetStart(), GetEnd(), TraceChannel, Params, ResponseParam);
}

bool UAdvKitTraceUtilityComponent::LineTraceTest(const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams)
{
	return GetWorld()->LineTraceTestByObjectType(GetStart(), GetEnd(), ObjectQueryParams, Params);
}

bool UAdvKitTraceUtilityComponent::LineTraceSingle(struct FHitResult& OutHit, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam)
{
	return GetWorld()->LineTraceSingleByChannel(OutHit, GetStart(), GetEnd(), TraceChannel, Params, ResponseParam);
}

bool UAdvKitTraceUtilityComponent::LineTraceSingle(struct FHitResult& OutHit, const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams)
{
	return GetWorld()->LineTraceSingleByObjectType(OutHit, GetStart(), GetEnd(), ObjectQueryParams, Params);
}

bool UAdvKitTraceUtilityComponent::LineTraceMulti(TArray<struct FHitResult>& OutHits, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam)
{
	return GetWorld()->LineTraceMultiByChannel(OutHits, GetStart(), GetEnd(), TraceChannel, Params, ResponseParam);
}

bool UAdvKitTraceUtilityComponent::LineTraceMulti(TArray<struct FHitResult>& OutHits, const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams)
{
	return GetWorld()->LineTraceMultiByObjectType(OutHits, GetStart(), GetEnd(), ObjectQueryParams, Params);
}

bool UAdvKitTraceUtilityComponent::SweepTest(const FQuat& Rot, ECollisionChannel TraceChannel, const struct FCollisionShape & CollisionShape, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam)
{
	return GetWorld()->SweepTestByChannel(GetStart(), GetEnd(), Rot, TraceChannel, CollisionShape, Params, ResponseParam);
}

bool UAdvKitTraceUtilityComponent::SweepTest(const FQuat& Rot, const struct FCollisionShape & CollisionShape, const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams)
{
	return GetWorld()->SweepTestByObjectType(GetStart(), GetEnd(), Rot, ObjectQueryParams, CollisionShape, Params);
}

bool UAdvKitTraceUtilityComponent::ComponentSweepMulti(TArray<struct FHitResult>& OutHits, class UPrimitiveComponent* PrimComp, const FRotator& Rot, const struct FComponentQueryParams& Params) 
{
	return GetWorld()->ComponentSweepMulti(OutHits, PrimComp, GetStart(), GetEnd(), Rot, Params);
}

bool UAdvKitTraceUtilityComponent::LineTraceSingleBlueprint(ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::LineTraceSingle_NEW(this, GetStart(), GetEnd(), TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::LineTraceMultiBlueprint(ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::LineTraceMulti_NEW(this, GetStart(), GetEnd(), TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::SphereTraceSingleBlueprint(float Radius, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::SphereTraceSingle_NEW(this, GetStart(), GetEnd(), Radius, TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::SphereTraceMultiBlueprint(float Radius, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::SphereTraceMulti_NEW(this, GetStart(), GetEnd(), Radius, TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::BoxTraceSingleBlueprint(const FVector HalfSize, const FRotator Orientation, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::BoxTraceSingle(this, GetStart(), GetEnd(), HalfSize, Orientation, TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::BoxTraceMultiBlueprint(FVector HalfSize, const FRotator Orientation, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::BoxTraceMulti(this, GetStart(), GetEnd(), HalfSize, Orientation, TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::CapsuleTraceSingleBlueprint(float Radius, float HalfHeight, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::CapsuleTraceSingle_NEW(this, GetStart(), GetEnd(), Radius, HalfHeight, TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::CapsuleTraceMultiBlueprint(float Radius, float HalfHeight, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::CapsuleTraceMulti_NEW(this, GetStart(), GetEnd(), Radius, HalfHeight, TraceChannel, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::LineTraceSingleForObjectBlueprints(const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::LineTraceSingleForObjects(this, GetStart(), GetEnd(), ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::LineTraceMultiForObjectsBlueprint(const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::LineTraceMultiForObjects(this, GetStart(), GetEnd(), ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::SphereTraceSingleForObjectBlueprints(float Radius, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::SphereTraceSingleForObjects(this, GetStart(), GetEnd(), Radius, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::SphereTraceMultiForObjectsBlueprint(float Radius, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::SphereTraceMultiForObjects(this, GetStart(), GetEnd(), Radius, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::BoxTraceSingleForObjectsBlueprint(const FVector HalfSize, const FRotator Orientation, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::BoxTraceSingleForObjects(this, GetStart(), GetEnd(), HalfSize, Orientation, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::BoxTraceMultiForObjectsBlueprint(const FVector HalfSize, const FRotator Orientation, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::BoxTraceMultiForObjects(this, GetStart(), GetEnd(), HalfSize, Orientation, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::CapsuleTraceSingleForObjectsBlueprint(float Radius, float HalfHeight, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::CapsuleTraceSingleForObjects(this, GetStart(), GetEnd(), Radius, HalfHeight, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit, bIgnoreSelf);
}

bool UAdvKitTraceUtilityComponent::CapsuleTraceMultiForObjects(float Radius, float HalfHeight, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf)
{
	return UKismetSystemLibrary::CapsuleTraceMultiForObjects(this, GetStart(), GetEnd(), Radius, HalfHeight, ObjectTypes, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHits, bIgnoreSelf);
}
