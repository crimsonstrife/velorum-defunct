#include "AdvKitRuntime.h"
#include "Player/AdvKitTargetLockComponent.h"
#include "Player/AdvKitCharacter.h"

const FName UAdvKitTargetLockComponent::SocketName(TEXT("TargetCameraViewpoint"));

UAdvKitTargetLockComponent::UAdvKitTargetLockComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Make the component tick after physics so the camera does not jitter
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	//Tick by default
	bAutoActivate = true;

	//Use all world coordinates
	bAbsoluteLocation = true;
	bAbsoluteRotation = true;
	bAbsoluteScale = true;

	//Default values
	MinimumDistanceFromCenter = 300.0f;
	CachedViewRotation = FQuat::Identity;
	TargetOffset = FVector(0, 0, 200);
}

void UAdvKitTargetLockComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto Character = GetOwningCharacter();
	auto Target = GetTarget();

	//Cannot update view without a character and target
	if (!Character || !Target)
	{
		return;
	}

	//Get world locations
	FVector CharacterLocation = Character->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	//Calculate center point
	FVector LocationDifference = TargetLocation - CharacterLocation;
	FVector HalfPosition = (CharacterLocation + TargetLocation)*0.5f;

	//Cache last location for smooth rotation (a kind of dragging the camera behind effect)
	FVector LastCameraLocation = GetSocketLocation(UAdvKitTargetLockComponent::SocketName);

	//Set new center location
	FVector NewBoomLocation = HalfPosition + TargetOffset;
	SetWorldLocation(NewBoomLocation);

	//Calculate distance from center 
	DistanceFromCenter = FMath::Max<float>(MinimumDistanceFromCenter, (TargetLocation - CharacterLocation).Size() / 1.5f);

	//Set new rotation and keep horizontal
	FVector BoomDiff = NewBoomLocation - LastCameraLocation;
	BoomDiff.Z = 0;

	//Set new rotation
	SetWorldRotation(BoomDiff.Rotation());

	//Cache current view rotation
	CachedViewRotation = ComponentToWorld.GetRotation().Inverse() * (HalfPosition - GetSocketLocation(USpringArmComponent::SocketName)).Rotation().Quaternion();
}

FTransform UAdvKitTargetLockComponent::GetSocketTransform(FName InSocketName, ERelativeTransformSpace TransformSpace) const
{
	//Socket is the distance from the center transformed by the rotation
	FVector LocalSocketOffset = -FVector::ForwardVector*DistanceFromCenter;
	
	//Depending on the query space, transform relative coordinates of the socket
	FTransform RelativeTransform(CachedViewRotation, LocalSocketOffset);
	switch(TransformSpace)
	{
		case RTS_World:
		{
			return RelativeTransform * ComponentToWorld;
			break;
		}
		case RTS_Actor:
		{
			if( const AActor* Actor = GetOwner() )
			{
				FTransform SocketTransform = RelativeTransform * ComponentToWorld;
				return SocketTransform.GetRelativeTransform(Actor->GetTransform());
			}
			break;
		}
		case RTS_Component:
		{
			return RelativeTransform;
		}
	}
	return RelativeTransform;
}

bool UAdvKitTargetLockComponent::HasAnySockets() const
{
	return true;
}

void UAdvKitTargetLockComponent::QuerySupportedSockets(TArray<FComponentSocketDescription>& OutSockets) const
{
	new (OutSockets) FComponentSocketDescription(SocketName, EComponentSocketType::Socket);
}

AAdvKitTargetPoint* UAdvKitTargetLockComponent::GetTarget()
{
	auto Character = GetOwningCharacter();
	if (!Character)
	{
		//No character means it can't have a target
		return nullptr;
	}

	return Character->CurrentTarget;
}	

AAdvKitCharacter* UAdvKitTargetLockComponent::GetOwningCharacter()
{
	return Cast<AAdvKitCharacter>(GetOwner());
}
