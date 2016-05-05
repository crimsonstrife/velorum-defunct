// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Environment/AdvKitTransitionComponent.h"
#include "Player/AdvKitCharacter.h"

UAdvKitTransitionComponent::UAdvKitTransitionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsDynamic = false;
	bDynamicTransitionActive = true;
	bNeedsJump = false;
	TransitionDirection = FVector::UpVector;
	MinDirectionNormal = 0.99f;
	MinLocationRadius = 2.0f;

	CreatedByBuildModule = nullptr;
}

FVector UAdvKitTransitionComponent::GetClosestSourcePosition(const FVector& ToWorldPosition)
{
	//Dummy implementation, implement in subclass
	return FVector::ZeroVector;
}

FVector UAdvKitTransitionComponent::GetClosestTargetPosition(const FVector& ToWorldPosition)
{
	//Dummy implementation, implement in subclass
	return FVector::ZeroVector;
}

bool UAdvKitTransitionComponent::CanTransition(AAdvKitCharacter* Character, const FVector& AtLocation, const FVector& InDirection, bool bIsJump)
{
	if (IsValid(UseableBy) && IsValid(Character))
	{
		if (!Character->IsA(UseableBy))
		{
			return false;
		}
	}

	if (bIsDynamic && !IsDynamicTransitionValid())
	{
		return false;
	}

	if (bNeedsJump != bIsJump)
	{
		return false;
	}

	auto* SourceZone = Cast<AAdvKitZone>(GetOuter());
	if (!SourceZone)
	{
		return false;
	}

	FVector GlobalDirection = SourceZone->GetTransform().TransformVectorNoScale(TransitionDirection);
	float Dot = FVector::DotProduct(InDirection.GetSafeNormal(), GlobalDirection.GetSafeNormal());
	//DrawDebugDirectionalArrow(GetWorld(), AtLocation, AtLocation + InDirection.GetSafeNormal() * 50, 5, FColor::Yellow, false, 2.0f, 0, 1);
	//DrawDebugDirectionalArrow(GetWorld(), AtLocation, AtLocation + GlobalDirection.GetSafeNormal()*100, 5, FColor::MakeRedToGreenColorFromScalar(Dot*0.5f + 0.5f), false, 2.0f, 0, 2);
	if (Dot < MinDirectionNormal)
	{
		return false;
	}

	FVector ClosestStart = GetClosestSourcePosition(AtLocation);
	if (FVector::Dist(AtLocation, ClosestStart) > MinLocationRadius)
	{
		return false;
	}

	return true;
}

bool UAdvKitTransitionComponent::IsDynamicTransitionValid()
{
	return bDynamicTransitionActive;
}

