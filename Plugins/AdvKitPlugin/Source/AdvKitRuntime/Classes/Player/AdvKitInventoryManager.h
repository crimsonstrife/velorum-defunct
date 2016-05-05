// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/Actor.h"
#include "AdvKitInventoryManager.generated.h"

class AAdvKitInventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemAddedSignature, class AAdvKitInventoryManager*, ToInventory, class AAdvKitInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FItemRemovedSignature, class AAdvKitInventoryManager*, FromInventory, class AAdvKitInventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActiveItemChanged, class AAdvKitInventoryManager*, InInventory, class AAdvKitInventoryItem*, NewItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemsChanged, class AAdvKitInventoryManager*, InInventory);

/**
 * @brief Inventory manager to hold all items a character carries.  
 */
UCLASS(/*Within = AdvKitCharacter*/)
class ADVKITRUNTIME_API AAdvKitInventoryManager : public AActor
{
	GENERATED_BODY()

protected:

	/** All items currently in the inventory */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", ReplicatedUsing=OnRep_InventoryArray)
	TArray<AAdvKitInventoryItem*> Inventory;

	/** Item the player wants to equip next */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", Replicated)
	AAdvKitInventoryItem* PendingInventory;

	/** Item that is currently equipped */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", Replicated)
	AAdvKitInventoryItem* CurrentInventory;

public:
	/** Constructor */
	AAdvKitInventoryManager(const FObjectInitializer& ObjectInitializer);

	//Begin AActor Interface
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	virtual void Tick(float DeltaSeconds) override;
	//End AActor Interface

	/**
	 * Getter for the pending inventory item that will be equipped next.
	 * @return Pending item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AAdvKitInventoryItem* GetPendingInventory() const;

	/**
	 * Getter for the currently equipped item.
	 * @return Current item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AAdvKitInventoryItem* GetCurrentInventory() const;

	/**
	 * Add a new item to the inventory e.g. when the character picks up a weapon.
	 * @param	NewInventory 	New item to add
	 * @param	bEquip 			Whether or not to equip the new item
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void AddInventory(AAdvKitInventoryItem* NewInventory, bool bEquip = false);
	virtual void AddInventory_Implementation(AAdvKitInventoryItem* NewInventory, bool bEquip = false);

	/**
	 * Remove an item from the inventory e.g. when the character drops a weapon.
	 * @param	RemovedInventory 	Item to remove
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void RemoveInventory(AAdvKitInventoryItem* RemovedInventory);
	virtual void RemoveInventory_Implementation(AAdvKitInventoryItem* RemovedInventory);

	/**
	 * Get the first inventory item that is of the given class.
	 * @param	InventoryClass 	Class of item to find
	 * @return	Item that matches class
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual AAdvKitInventoryItem* GetInventoryByClass(TSubclassOf<AAdvKitInventoryItem> InventoryClass);

	/**
	 * Cycle through the inventory array and equip the next item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual AAdvKitInventoryItem* NextItem();

	/**
	 * Cycle through the inventory array and equip the previous item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual AAdvKitInventoryItem* PreviousItem();

	/**
	 * Check to see if an item can be equipped
	 * @param	NewInventory 	The item to equip
	 * @param	bInstant 		False to play un-/equip animation, true to equip without delay
	 * @return True if it can be equipped, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual bool CanEquipInventory(AAdvKitInventoryItem* NewInventory, bool bInstant = false);

	/**
	 * Equip an item
	 * @param	NewInventory 	The item to equip
	 * @param	bInstant 		False to play un-/equip animation, true to equip without delay
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void SetNewInventory(AAdvKitInventoryItem* NewInventory, bool bInstant = false);

	/**
	 * Equip an item that is of the specified class
	 * @param	NewInventory 	The class of the item to equip
	 * @param	bInstant 		False to play un-/equip animation, true to equip without delay
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void SetNewInventoryByClass(TSubclassOf<AAdvKitInventoryItem> NewInventoryClass, bool bInstant = false);

	/**
	 * Server call to equip an item that is of the specified class
	 * @param	NewInventory 	The class of the item to equip
	 * @param	bInstant 		False to play un-/equip animation, true to equip without delay
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void SetNewInventoryServer(AAdvKitInventoryItem* NewInventory, bool bInstant = false);
	virtual void SetNewInventoryServer_Implementation(AAdvKitInventoryItem* NewInventory, bool bInstant = false);

	/**
	 * Callback when the owner character of the inventory manager died.
	 */
	virtual void OwnerDied();

	/**
	* Delegate that is called when an item was added to the inventory
	*/
	UPROPERTY(Category = "Inventory", BlueprintAssignable)
	FItemAddedSignature OnItemAdded_DEPRECATED;
	
	/**
	 * Delegate that is called when an item was removed from the inventory
	 */
	UPROPERTY(Category = "Inventory", BlueprintAssignable)
	FItemRemovedSignature OnItemRemoved_DEPRECATED;

	/**
	 * Delegate that is called when a new item was equipped
	 */
	UPROPERTY(Category = "Inventory", BlueprintAssignable)
	FActiveItemChanged OnActiveItemChanged;

	/**
	 * Delegate that is called when a new item was added or removed
	 */
	UPROPERTY(Category = "Inventory", BlueprintAssignable)
	FInventoryItemsChanged OnInventoryItemsChanged;

protected:

	/**
	 * Callback when the inventory array changed remotely. 
	 */
	UFUNCTION()
	virtual void OnRep_InventoryArray();
};
