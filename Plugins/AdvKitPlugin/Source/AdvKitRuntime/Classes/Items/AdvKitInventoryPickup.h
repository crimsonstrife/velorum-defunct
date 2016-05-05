// Copyright 2015 Pascal Krabbe

#pragma once

#include "Items/AdvKitPickup.h"
#include "AdvKitInventoryPickup.generated.h"

/**
 * @brief Inventory pickups spawn items when overlapped or used by the character and add it to 
 * the character's inventory.
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitInventoryPickup : public AAdvKitPickup
{
	GENERATED_BODY()
protected:
	/** The item that is spawned for the character on pickup. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
	TSubclassOf<class AAdvKitInventoryItem> SpawnedInventoryClass;

public:
	/**
	 * Constructor
	 */
	AAdvKitInventoryPickup(const FObjectInitializer& ObjectInitializer);

	//Begin AAdvKitPickup Interface
	virtual void PickedUpBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL) override;
	//End AAdvKitPickup Interface

};
