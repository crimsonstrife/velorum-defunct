// Copyright 2015 Pascal Krabbe

#pragma once

#include "Items/AdvKitUsable.h"
#include "AdvKitPickup.generated.h"

/**
 * @brief Pickup objects can act when the player overlaps them or uses them.
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitPickup : public AAdvKitUsable
{
	GENERATED_BODY()
protected:	
	/** Destroy this pickup actor after being picked up by a character. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pickup)
	bool bDestroyOnPickup;

	/** Respawn after being picked up. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pickup, meta = (EditCondition = "!bDestroyOnPickup"))
	bool bRespawnAfterPickup;

	/** How long to wait until respawning. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pickup, meta = (EditCondition = "bRespawnAfterPickup"))
	float RespawnTime;

	/** Whether this pickup is currently active (and visible etc...). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pickup)
	bool bActive;

	/** Can this be picked up with the UsedBy function? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pickup)
	bool bCanBePickedUpWithUse;

	/** Timer to manage respawning interval (if pickup respawns). */
	FTimerHandle RespawnTimerHandle;

public:
	/**
	 * Constructor
	 */
	AAdvKitPickup(const FObjectInitializer& ObjectInitializer);

	//Begin AActor Interface
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;
	//End AActor Interface

	/**
	 * Called by a timer after RespawnTime
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void Respawn();

	/**
	 * Can a character pick this pickup up? (what a question indeed...)
	 * 
	 * @param	Character	The character that wants to pick up this object.
	 * @return				True if character can pick up the object.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AdvKit")
	bool CanBePickedUpBy(class AAdvKitCharacter* Character);
	virtual bool CanBePickedUpBy_Implementation(class AAdvKitCharacter* Character);


protected:
	/**
	 * Picked up by a character (should only be called when CanBePickedUpBy returns true)
	 *
	 * @param	Character	The character that picked up this object.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AdvKit", meta=(BlueprintProtected))
	void PickedUpBy(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL);
	virtual void PickedUpBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL);

public:

	//Begin AAdvKitUsable Interface
	virtual bool CanBeUsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL) override;
	virtual void UsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL) override;
	//End AAdvKitUsable Interface
};
