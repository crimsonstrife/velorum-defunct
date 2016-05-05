// Copyright 2015 Pascal Krabbe

#pragma once

#include "Items/AdvKitInventoryItem.h"
#include "AdvKitInventorySimpleCombinable.generated.h"

/**
 * @brief Simple class for combination of two items. If successful both items are destroyed and a new item will be spawned.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API AAdvKitInventorySimpleCombinable : public AAdvKitInventoryItem
{
	GENERATED_BODY()

protected:
	/** Class of another item that this item can be combined with. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AdvKit)
	TSubclassOf<class AAdvKitInventoryItem> CombinableWithClass;

	/** Result of a successful combination of items. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AdvKit)
	TSubclassOf<class AAdvKitInventoryItem> CombinationResultClass;

public:
	/**
	 * Constructor
	 */
	AAdvKitInventorySimpleCombinable(const FObjectInitializer& ObjectInitializer);

	//Begin AAdvKitInventoryItem Interface
	virtual bool CanBeCombinedWith_Implementation(class AAdvKitInventoryItem* With) override;
	virtual class AAdvKitInventoryItem* CombinedWith_Implementation(class AAdvKitInventoryItem* With) override;
	//End AAdvKitInventoryItem Interface
};
