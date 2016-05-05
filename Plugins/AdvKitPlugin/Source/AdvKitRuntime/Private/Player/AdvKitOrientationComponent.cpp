

#include "AdvKitRuntime.h"
#include "Player/AdvKitOrientationComponent.h"

UAdvKitOrientationComponent::UAdvKitOrientationComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PostPhysics;

	bWantsInitializeComponent = true;
	SetIsReplicated(true);
	bAutoRegister = true;
	bNetAddressable = true;

	LocationLerpSpeed = 10;
	RotationLerpSpeed = 10;

	bAbsoluteLocation = true;
	bAbsoluteRotation = true;
	bAbsoluteScale = true;

	TargetTransform = FTransform();
}

FVector UAdvKitOrientationComponent::TransformDirectionW2L(const FVector& Direction)
{
	return GetComponentQuat().Inverse().RotateVector(Direction);
}

FVector UAdvKitOrientationComponent::TransformDirectionL2W(const FVector& Direction)
{
	return GetComponentQuat().RotateVector(Direction);
}

FVector UAdvKitOrientationComponent::TransformPositionW2L(const FVector& Position)
{
	return GetComponentTransform().InverseTransformPositionNoScale(Position);

}

FVector UAdvKitOrientationComponent::TransformPositionL2W(const FVector& Position)
{
	return GetComponentTransform().TransformPositionNoScale(Position);
}

FRotator UAdvKitOrientationComponent::TransformRotationW2L(const FRotator& Rotation)
{
	return TransformRotationW2L(Rotation.Quaternion()).Rotator();
	//return Rotation - GetComponentRotation();
}

FRotator UAdvKitOrientationComponent::TransformRotationL2W(const FRotator& Rotation)
{
	return TransformRotationL2W(Rotation.Quaternion()).Rotator();
	//return Rotation + GetComponentRotation();
}

FQuat UAdvKitOrientationComponent::TransformRotationW2L(const FQuat&  Rotation)
{
	return GetComponentQuat().Inverse()*Rotation;
}

FQuat UAdvKitOrientationComponent::TransformRotationL2W(const FQuat&  Rotation)
{
	return GetComponentQuat()*Rotation;
}

void UAdvKitOrientationComponent::SetOrientation(const FVector Origin, const FVector UpNormal, bool bForce)
{
	FVector Up = UpNormal.GetSafeNormal();

	if (GetUpVector().Equals(Up) && GetComponentLocation().Equals(Origin))
	{
		return;
	}

	//Conjure up a coordinate system from the up vector
	FVector HelpVector = FVector::UpVector;
	if (FMath::Abs<float>(FVector::DotProduct(Up, FVector::UpVector)) > 0.95f)
	{
		HelpVector = FVector::ForwardVector;
	}

	FVector Right, Forward;
	Up.FindBestAxisVectors(Right, Forward);

	//Create a new orientation from the axes
	FQuat TargetQuat = FMatrix(Forward, Right, Up, FVector::ZeroVector).ToQuat();

	//TargetQuat would be enough to construct a new orientation from, however in order to get a smooth
	//interpolation between the old and the new the rotation needs to be aligned to the old one as best 
	//as possible.
	FQuat OldQuat = GetComponentQuat();

	//First the old quaternion needs to be transformed into the new quaternion space
	FQuat LocalOldQuat = TargetQuat.Inverse() * OldQuat;

	//Then the now local space rotation needs to be aligned to a planar rotation
	FVector LocalOldForward = LocalOldQuat.RotateVector(FVector::ForwardVector);
	FVector LocalPlanarForward = LocalOldForward;
	LocalPlanarForward.Z = 0;

	FRotator LocalForwardRotation = LocalPlanarForward.Rotation();

	//After the planar operation the new rotation is transformed back into world space
	FQuat NewQuat = TargetQuat * LocalForwardRotation.Quaternion();

	//The transformation is now aligned to the new floor and as close as possible same to the old one
	TargetTransform = FTransform(NewQuat, Origin, FVector(1, 1, 1));

	if (bForce || FMath::IsNearlyZero(RotationLerpSpeed))
	{
		UpdateTransform(TargetTransform);
	}
}


void UAdvKitOrientationComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Lerp it to get a smooth transition
	FTransform OldTransform = GetComponentTransform();
	FVector LerpedOrigin = FMath::Lerp<FVector>(OldTransform.GetLocation(), TargetTransform.GetLocation(), FMath::Min<float>(1, LocationLerpSpeed*DeltaTime));
	FQuat LerpedRotation = FQuat::FastLerp(OldTransform.GetRotation(), TargetTransform.GetRotation(), FMath::Min<float>(1, RotationLerpSpeed*DeltaTime));

	//Apply the new interpolated transformation
	FTransform LerpedTransform(LerpedRotation, LerpedOrigin, FVector(1, 1, 1));
	UpdateTransform(LerpedTransform);
}

void UAdvKitOrientationComponent::UpdateTransform(const FTransform& NewTransform)
{
	FTransform OldTransform = GetComponentTransform();
	SetWorldTransform(NewTransform);

	if (OnOrientationChanged.IsBound())
	{
		OnOrientationChanged.Broadcast(OldTransform);
	}
}
