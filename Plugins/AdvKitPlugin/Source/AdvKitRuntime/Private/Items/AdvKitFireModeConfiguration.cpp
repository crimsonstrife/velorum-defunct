// Copyright 2015 Pascal Krabbe

#include "AdvKitRuntime.h"
#include "Items/AdvKitFireModeConfiguration.h"
#include "Items/AdvKitWeapon.h"

#include "Net/UnrealNetwork.h"

UAdvKitFireModeConfiguration::UAdvKitFireModeConfiguration(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AmmunitionPerShot = 1;
	InstantDamage = 10;
	TraceDistance = 10000;

	bShouldRefire = true;
	RefireTime = 0.5f;

	//bAutoActivate = true;
	//SetNetAddressable();
	//SetIsReplicated(true);
}

#if WITH_EDITOR
bool UAdvKitFireModeConfiguration::CanEditChange(const UProperty* Property) const
{
	bool bIsEditable = Super::CanEditChange(Property);
	//if (bIsEditable && Property != NULL)
	//{
	//	if (Property->GetFName() == TEXT("ProjectileClass"))
	//	{
	//		return FireType == EAdvKitRifleFireType::Projectile;
	//	}
	//}

	return bIsEditable;
}
#endif
