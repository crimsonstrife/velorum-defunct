// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Actions/Movement/AdvKitCA_ZoneTransition_SingleAnim.h"

UAnimMontage* UAdvKitCA_ZoneTransition_SingleAnim::GetMontageToPlay_Implementation()
{
	return Montage;
}
