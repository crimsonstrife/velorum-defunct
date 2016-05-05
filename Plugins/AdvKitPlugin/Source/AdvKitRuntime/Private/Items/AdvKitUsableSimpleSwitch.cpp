// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"

#include "Items/AdvKitInventoryItem.h"
#include "Items/AdvKitUsableSimpleSwitch.h"

#include "Actions/Interaction/AdvKitCA_UseSwitch.h"
#include "Actions/Interaction/AdvKitCA_UseSwitch_Arguments.h"

#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitCharacterActionManager.h"

#include "Net/UnrealNetwork.h"


AAdvKitUsableSimpleSwitch::AAdvKitUsableSimpleSwitch(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	KeyClass = NULL;
	bActive = false;
	bCanBeActivated = true;
	bCanBeDeactivated = true;
	bReplicates = true;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);

	Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, "Root");
	RootComponent = Root;
}

void AAdvKitUsableSimpleSwitch::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAdvKitUsableSimpleSwitch, bActive);
}


bool AAdvKitUsableSimpleSwitch::CanBeUsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem)
{
	if (bActive && !bCanBeDeactivated)
	{
		return false;
	}

	if (!bActive && !bCanBeActivated)
	{
		return false;
	}

	if (!KeyClass)
	{
		return true;
	}

	if (!WithItem)
	{
		return false;
	}

	return WithItem->IsA(KeyClass);
}

void AAdvKitUsableSimpleSwitch::UsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem)
{
	if (!CanBeUsedBy(Character, WithItem))
	{
		return;
	}

	//Activate the switch
	auto ActiveAction = Character->ActionManager->GetActiveAction();
	if (!bActive && ActivationCharacterActionClass)
	{
		if (!ActiveAction || !ActiveAction->IsA(ActivationCharacterActionClass))
		{
			StartSwitchAction(ActivationCharacterActionClass, Character, WithItem);
			return;
		}
	}

	//Deactivate the switch
	if (bActive && DeactivationCharacterActionClass)
	{
		if (!ActiveAction || !ActiveAction->IsA(DeactivationCharacterActionClass))
		{
			StartSwitchAction(DeactivationCharacterActionClass, Character, WithItem);
			return;
		}
	}

	Super::UsedBy_Implementation(Character, WithItem);

	//Broadcast activation changes
	bActive = !bActive;
	if (bActive)
	{
		if (OnActivatedBy.IsBound())
		{
			OnActivatedBy.Broadcast(Character, WithItem);
		}
	}
	else
	{
		if (OnDeactivatedBy.IsBound())
		{
			OnDeactivatedBy.Broadcast(Character, WithItem);
		}
	}
}

void AAdvKitUsableSimpleSwitch::StartSwitchAction(TSubclassOf<class UAdvKitCA_UseSwitch> ActionClass, class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem)
{
	Character->StartAction(ActionClass, UAdvKitCA_UseSwitch::MakeArguments(this, WithItem));
}