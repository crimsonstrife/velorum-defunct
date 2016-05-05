

#include "AdvKitRuntime.h"

#include "Actions/Interaction/AdvKitCA_UseSwitch.h"
#include "Actions/Interaction/AdvKitCA_UseSwitch_Arguments.h"

#include "Items/AdvKitUsableSimpleSwitch.h"

#include "Player/AdvKitCharacter.h"
#include "Player/AdvKitCharacterMovementComponent.h"


FAdvKitActionResponse UAdvKitCA_UseSwitch::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	//Need arguments to use switch
	UAdvKitCA_UseSwitch_Arguments* SwitchArgs = Cast<UAdvKitCA_UseSwitch_Arguments>(Arguments);
	if (!IsValid(SwitchArgs))
	{
		return Failed();
	}

	UseSwitch = SwitchArgs->UseSwitch;
	UseWithItem = SwitchArgs->UseWithItem;

	//If no switch was supplied or switch cannot be used, this action cannot be started
	if (!UseSwitch || !UseSwitch->CanBeUsedBy(CharacterOwner, UseWithItem))
	{
		return Failed();
	}

	//Play animation and apply start configuration to character
	auto Result = Super::Start_Implementation(Arguments);
	
	//Use switch
	UseSwitch->UsedBy(CharacterOwner, UseWithItem);

	return Result;
}

class UAdvKitCharacterAction_Arguments* UAdvKitCA_UseSwitch::MakeArguments(class AAdvKitUsableSimpleSwitch* Switch, class AAdvKitInventoryItem* WithItem)
{
	UAdvKitCA_UseSwitch_Arguments* Arguments = NewObject<UAdvKitCA_UseSwitch_Arguments>();
	Arguments->UseSwitch = Switch;
	Arguments->UseWithItem = WithItem;

	return Arguments;
}
