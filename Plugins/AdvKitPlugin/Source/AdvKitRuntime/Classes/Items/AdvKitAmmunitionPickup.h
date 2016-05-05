// Copyright 2015 Pascal Krabbe

#pragma once

#include "Items/AdvKitPickup.h"
#include "AdvKitAmmunitionPickup.generated.h"

/**
 * @brief Pickup class that provides ammunition for a specified weapon class.
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitAmmunitionPickup : public AAdvKitPickup
{
	GENERATED_BODY()

protected:
	/** The weapon this supply is for. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TSubclassOf<class AAdvKitWeapon> AmmuntionForWeapon;

	/** How much ammunition the pickup gives. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	int32 Ammunition;

public:
	/**
	 * Constructor
	 */
	AAdvKitAmmunitionPickup(const FObjectInitializer& ObjectInitializer);

	//Begin AAdvKitPickup Interface
	virtual void PickedUpBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL) override;
	//End AAdvKitPickup Interface

};
