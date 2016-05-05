// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Items/AdvKitInventorySimpleCombinable.h"

AAdvKitInventorySimpleCombinable::AAdvKitInventorySimpleCombinable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CombinableWithClass = NULL;
	CombinationResultClass = NULL;
}


bool AAdvKitInventorySimpleCombinable::CanBeCombinedWith_Implementation(class AAdvKitInventoryItem* With)
{
	if (!CombinableWithClass)
	{
		return false;
	}

	if (!With)
	{
		return false;
	}

	if (With == this)
	{
		return false;
	}

	return With->IsA(CombinableWithClass);
}

class AAdvKitInventoryItem* AAdvKitInventorySimpleCombinable::CombinedWith_Implementation(class AAdvKitInventoryItem* With)
{
	if (!CanBeCombinedWith(With))
	{
		return NULL;
	}

	if (!CombinationResultClass)
	{
		return NULL;
	}

	AAdvKitInventoryItem* NewInventory = GetWorld()->SpawnActor<AAdvKitInventoryItem>(CombinationResultClass);
	if (!NewInventory)
	{
		return NULL;
	}

	With->Destroy();
	Destroy();
	
	return NewInventory;
}
