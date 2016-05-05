// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/AdvKitCharacterActionSingleAnimation.h"


FAdvKitActionResponse UAdvKitCharacterActionSingleAnimation::Start_Implementation(class UAdvKitCharacterAction_Arguments* Arguments, UAdvKitCharacterAction* InterruptedOther)
{
	FAdvKitActionResponse SuperResponse = Super::Start_Implementation(Arguments, InterruptedOther);

	if (!AnimMontage)
	{
		UE_LOG(LogAdvKit, Error, TEXT("UAdvKitCharacterActionSingleAnimation::Start No AnimMontage for animation action %s"), *GetName());
		return Stop();
	}

	PlayAnimMontage(AnimMontage);
	return SuperResponse;
}

void UAdvKitCharacterActionSingleAnimation::OnMontageEnded_Implementation(UAnimMontage *EndedMontage, bool bInterrupted)
{
	if (EndedMontage != AnimMontage)
	{
		return;
	}

	if (!IsActive())
	{
		//Already ended
		return;
	}

	Stop();
}

bool UAdvKitCharacterActionSingleAnimation::CanBeInterruptedBy_Implementation(const UAdvKitCharacterAction* Other) const
{
	return false;
}
