#include "AdvKitRuntime.h"
#include "Actions/AdvKitCharacterAction_Arguments.h"

UAdvKitCharacterAction_Arguments* UAdvKitCharacterAction_Arguments::CreateArguments(UObject* WorldContextObject, TSubclassOf<UAdvKitCharacterAction_Arguments> ArgumentClass)
{
	return NewObject<UAdvKitCharacterAction_Arguments>(WorldContextObject, ArgumentClass);
}
