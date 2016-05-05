#include "AdvKitRuntime.h"
#include "Player/AdvKitTargetPoint.h"

AAdvKitTargetPoint::AAdvKitTargetPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCanBeTargeted = true;
}

AActor* AAdvKitTargetPoint::GetTargetedActor()
{
	//Check if target actor is attached to another actor to avoid targeting loose traget points
	auto ParentComp = GetParentComponent();
	if (!ParentComp)
	{
		return nullptr;
	}

	return ParentComp->GetOwner();
}

bool AAdvKitTargetPoint::IsValidTarget()
{
	return bCanBeTargeted;
}
