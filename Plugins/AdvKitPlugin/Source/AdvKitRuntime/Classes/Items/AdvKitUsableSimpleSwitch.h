// Copyright 2015 Pascal Krabbe

#pragma once

#include "Items/AdvKitUsable.h"
#include "AdvKitUsableSimpleSwitch.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSwitchActivatedBySignature, class AAdvKitCharacter*, ByCharacter, class AAdvKitInventoryItem*, WithItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSwitchDeactivatedBySignature, class AAdvKitCharacter*, ByCharacter, class AAdvKitInventoryItem*, WithItem);

/**
 * @brief Base class for a simple switch that can be activated with an item (e.g. a key)
 */
UCLASS(abstract)
class ADVKITRUNTIME_API AAdvKitUsableSimpleSwitch : public AAdvKitUsable
{
	GENERATED_BODY()

protected:
	/** Root component for useable actors. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	/** Class of another item that this switch needs to be activated (if any). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Switch)
	TSubclassOf<class AAdvKitInventoryItem> KeyClass;

	/** Is this switch currently active? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = Switch)
	bool bActive;

	/** Can this switch be activated? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Switch)
	bool bCanBeActivated;

	/** Can this switch be deactivated? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Switch)
	bool bCanBeDeactivated;

public:
	/** Delegate that is called when the switch was activated. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AdvKit")
	FSwitchActivatedBySignature OnActivatedBy;
	
	/** Delegate that is called when the switch was deactivated. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AdvKit")
	FSwitchDeactivatedBySignature OnDeactivatedBy;

	/** Action the player has to do when activating the switch. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Switch)
	TSubclassOf<class UAdvKitCA_UseSwitch> ActivationCharacterActionClass;

	/** Action the player has to do when deactivating the switch. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Switch)
	TSubclassOf<class UAdvKitCA_UseSwitch> DeactivationCharacterActionClass;

public:
	/**
	 * Constructor
	 */
	AAdvKitUsableSimpleSwitch(const FObjectInitializer& ObjectInitializer);

	/**
	 * Starts the an action on the using character (in order to make it look like the character is using the switch).
	 * @param	ActionClass	Class of the action to start
	 * @param	Character	Character to start the action on
	 * @param	WithItem	Item to use with the switch
	 */
	virtual void StartSwitchAction(TSubclassOf<class UAdvKitCA_UseSwitch> ActionClass, class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL);

	//Begin AAdvKitUsable Interface
	virtual void UsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL) override;
	virtual bool CanBeUsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL) override;
	//End AAdvKitUsable Interface

};
