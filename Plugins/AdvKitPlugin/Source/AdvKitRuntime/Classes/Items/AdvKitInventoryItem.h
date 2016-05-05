// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/Actor.h"
#include "AdvKitInventoryItem.generated.h"

/**
 * @brief Enumeration to specify state of item
 */
UENUM(BlueprintType)
enum class EAdvKitInventoryState : uint8
{
	Inactive,
	Equipping,
	Active,
	Uneqiupping,
	Unknown
};

/**
 * @brief Struct to handle all replicated state variables of an item.
 */
USTRUCT()
struct FRepAdvKitInventoryState
{
	GENERATED_USTRUCT_BODY();

	/** Current state of the item */
	UPROPERTY()
	EAdvKitInventoryState State;

	/** Whether or not the state change is instant */
	UPROPERTY()
	bool bInstantChange;

	/** Item should be equipped as soon as possible */
	UPROPERTY()
	bool bWantsToEquip;

	/** Item should be unequipped as soon as possible */
	UPROPERTY()
	bool bWantsToUnequip;

	FRepAdvKitInventoryState()
	{
		State = EAdvKitInventoryState::Inactive;
		bInstantChange = false;
		bWantsToEquip = false;
		bWantsToUnequip = false;
	};
};

/**
 * @brief Base item class that defines objects that the character can use
 * and store in its inventory. Items can be equipped and unequipped.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API AAdvKitInventoryItem : public AActor
{
	GENERATED_BODY()
public:

	/** Name of this inventory item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText ItemName;

	/** Flavor text for the item. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FText ItemDescription;

	/** Icon to draw for this inventory item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
	UTexture* Icon;

	/** Pickup that is dropped when this item is dropped by the owning character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<class AAdvKitPickup> DroppedPickupClass;

	/** Flag to drop item when owner died */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bDropOnDeath; 

	/** Whether or not this item needs aim offsets (e.g. guns for targeting) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bUseAimOffset;

	/** How long does it equip this item (in Seconds)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	float EquipTime;

	/** Montage to play on the character when equipping this item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	UAnimMontage* EquipMontage;

	/** How long does it take to unequip this item (in Seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	float UneqiupTime;

	/** Montage to play on the character when unequipping this item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	UAnimMontage* UneqiupMontage;
	
	/** Name of the socket on the character mesh to attach the inventory to when equipping. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	FName AttachmentSocketName;

	/** Timer to fire when equipping is done. */
	FTimerHandle EquipTimerHandle;

public:
	/**
	 * Constructor
	 */
	AAdvKitInventoryItem(const FObjectInitializer& ObjectInitializer);

	//Begin AActor Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//End AActor Interface

	/**
	 * Can this item be combined with another?
	 *
	 * @param	With	The other item to combine with.
	 * @return			True if combinable.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	bool CanBeCombinedWith(class AAdvKitInventoryItem* With);
	virtual bool CanBeCombinedWith_Implementation(class AAdvKitInventoryItem* With);

	/**
	 * Combine this item with another.
	 *
	 * @param	With	The other item to combine with.
	 * @return			The result of the combination.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	class AAdvKitInventoryItem* CombinedWith(class AAdvKitInventoryItem* With);
	virtual class AAdvKitInventoryItem* CombinedWith_Implementation(class AAdvKitInventoryItem* With);

	/**
	 * Event called when the owning character died.
	 */
	virtual void OwnerDied();

	/**
	 * Drop this item (spawns pickup if configured).
	 * @param	Velocity	In which direction to drop the pickup.
	 */
	virtual void Drop(FVector Velocity = FVector::ZeroVector);

protected:
	/**
	 * Attaches this item to the character (e.g. a weapon to the hand).
	 * Called when item is being equipped.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void AttachToCharacter();
	virtual void AttachToCharacter_Implementation();

	/**
	 * Detaches this item from the character (e.g. a weapon frmo the hand).
	 * Called when item is being unequipped.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	void DetachFromCharacter();
	virtual void DetachFromCharacter_Implementation();

	/** State Handling */

	/** Replicated state of the item. */
	UPROPERTY(ReplicatedUsing = OnRep_ItemState)
	FRepAdvKitInventoryState RepItemState;

	/**
	 * Set a new item state
	 * @param	NewState 	The new state to set
	 * @param	bInstant 	Whether to change states instant or do the regular transitions
	 */
	void SetItemState(EAdvKitInventoryState NewState, bool bInstant = false);

	/**
	 * Callback when item state changed remotely. 
	 * @param	PreviousRepItemState	State value before the change.
	 */
	UFUNCTION()
	virtual void OnRep_ItemState(FRepAdvKitInventoryState PreviousRepItemState);

public:
	/**
	 * Getter for the inventory item state.
	 * @return	State struct of the item.
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	EAdvKitInventoryState GetItemState();

	/**
	 * Tell the item to equip.
	 * @param	bInstant 	True to equip instantly, otherwise regular equipment time and animations
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void Equip(bool bInstant = false);

	/**
	 * Tell the item to unequip.
	 * @param	bInstant 	True to unequip instantly, otherwise regular unequipment time and animations
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual void Uneqiup(bool bInstant = false);

	/**
	 * Check if the item can be equipped.
	 * @param	bInstant	Can it be equipped instantly.
	 * @return	True if item can be equipped.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	bool CanEquip(bool bInstant = false);
	virtual bool CanEquip_Implementation(bool bInstant = false);

	/**
	 * Check if the item can be unequipped.
	 * @param	bInstant	Can it be unequipped instantly.
	 * @return	True if item can be unequipped.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	bool CanUneqiup(bool bInstant = false);
	virtual bool CanUneqiup_Implementation(bool bInstant = false);

protected:

	/**
	 * Callback when the item was equipped (either by timer or instant)
	 * @param	bInstant 	True if the equip was instant
	 */
	virtual void Equipped(bool bInstant = false);

	/**
	 * Callback when the item was unequipped (either by timer or instant)
	 * @param	bInstant 	True if the unequip was instant
	 */
	virtual void Uneqiupped(bool bInstant = false);

	/**
	 * Server callback to tell the item to unequip.
	 * @param	bInstant 	True to equip instantly, otherwise regular unequipment time and animations
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void UneqiupServer(bool bInstant = false);
	virtual void UneqiupServer_Implementation(bool bInstant = false);

	/**
	 * Server callback to tell the item to equip.
	 * @param	bInstant 	True to equip instantly, otherwise regular equipment time and animations
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void EquipServer(bool bInstant = false);
	virtual void EquipServer_Implementation(bool bInstant = false);

	/** State Callbacks*/

	/**
	 * Event called when item starts to equip.
	 * @param	bInstant	Whether equipping is instant or not
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory", meta = (DisplayName = "On Equip Begin" ))
	void OnEquip(bool bInstant = false);
	virtual void OnEquip_Implementation(bool bInstant = false);
	
	/**
	 * Event called when item finished equipping.
	 * @param	bInstant	Whether equipping was instant or not
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory", meta = (DisplayName = "On Equip Finished"))
	void OnEquipped(bool bInstant = false);
	virtual void OnEquipped_Implementation(bool bInstant = false);

	/**
	 * Event called when item starts to unequip.
	 * @param	bInstant	Whether unequipping is instant or not
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory", meta = (DisplayName = "On Uneqiup Begin"))
	void OnUneqiup(bool bInstant = false);
	virtual void OnUneqiup_Implementation(bool bInstant = false);
	
	/**
	 * Event called when item finished unequipping.
	 * @param	bInstant	Whether unequipping was instant or not
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Inventory", meta = (DisplayName = "On Uneqiup Finished"))
	void OnUneqiupped(bool bInstant = false);
	virtual void OnUneqiupped_Implementation(bool bInstant = false);
};
