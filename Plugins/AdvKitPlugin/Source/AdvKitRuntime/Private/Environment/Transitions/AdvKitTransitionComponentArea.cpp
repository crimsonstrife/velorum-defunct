#include "AdvKitRuntime.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"
#include "Environment/AdvKitZone.h"

void UAdvKitTransitionComponentArea::Init(TSubclassOf<AAdvKitCharacter> ForCharacter, const FVector& MinWorldSource, const FVector& MinWorldTarget, const FVector& MaxWorldSource, const FVector& MaxWorldTarget, EAdvKitMovementMode NewTargetPhysics, class AAdvKitZone* TargetZone, uint8 NewTargetCustomPhysics)
{
	AActor* BaseActor = Cast<AActor>(this->GetOuter());

	MinSourcePosition = FBasedPosition(BaseActor, MinWorldSource);
	MaxSourcePosition = FBasedPosition(BaseActor, MaxWorldSource);

	MinTargetPosition = FBasedPosition(TargetZone ? TargetZone : BaseActor, MinWorldTarget);
	MaxTargetPosition = FBasedPosition(TargetZone ? TargetZone : BaseActor, MaxWorldTarget);

	FVector MediumSource = (MinWorldSource + MaxWorldSource)*0.5f;
	FVector MediumTarget = (MinWorldTarget + MaxWorldTarget)*0.5f;
	TransitionDirection = BaseActor->GetTransform().InverseTransformVector(MediumTarget - MediumSource).GetSafeNormal();

	UseableBy = ForCharacter;

	this->TargetZone = TargetZone;
	TargetPhysics = NewTargetPhysics;
	TargetCustomPhysics = NewTargetCustomPhysics;

	auto BaseRoot = BaseActor->GetRootComponent();
	if (BaseRoot && BaseRoot->Mobility == EComponentMobility::Movable)
	{
		bIsDynamic = true;
	}

	if (TargetZone)
	{
		auto TargetRoot = TargetZone->GetRootComponent();
		if (TargetRoot && TargetRoot->Mobility == EComponentMobility::Movable)
		{
			bIsDynamic = true;
		}
	}
}

FVector UAdvKitTransitionComponentArea::GetClosestSourcePosition(const FVector& ToWorldPosition)
{
	return FMath::ClosestPointOnLine(*MinSourcePosition, *MaxSourcePosition, ToWorldPosition);
}

FVector UAdvKitTransitionComponentArea::GetClosestTargetPosition(const FVector& ToWorldPosition)
{
	return FMath::ClosestPointOnLine(*MinTargetPosition, *MaxTargetPosition, ToWorldPosition);
}

void UAdvKitTransitionComponentArea::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	//Clean up all pointers
	MinSourcePosition.Clear();
	MaxSourcePosition.Clear();
	MinTargetPosition.Clear();
	MaxTargetPosition.Clear();
}


FVector UAdvKitTransitionComponentArea::GetMinSourceLocation() const
{
	return *MinSourcePosition;
}

FVector UAdvKitTransitionComponentArea::GetMaxSourceLocation() const
{
	return *MaxSourcePosition;
}

FVector UAdvKitTransitionComponentArea::GetMinTargetLocation() const 
{
	return *MinTargetPosition;
}

FVector UAdvKitTransitionComponentArea::GetMaxTargetLocation() const 
{
	return *MaxTargetPosition;
}


