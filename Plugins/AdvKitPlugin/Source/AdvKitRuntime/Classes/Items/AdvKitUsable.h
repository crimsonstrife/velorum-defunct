// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/Actor.h"
#include "AdvKitUsable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUsedBySignature, class AAdvKitCharacter*, ByCharacter, class AAdvKitInventoryItem*, WithItem);

/**
 * @brief Base class for all sorts of usable scene actors (like levers or buttons).
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitUsable : public AActor
{
	GENERATED_BODY()
public:
	/**
	 * Constructor
	 */
	AAdvKitUsable(const FObjectInitializer& ObjectInitializer);

	/** Delegate that is called when this actor was used. */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "AdvKit")
	FUsedBySignature OnUsedBy;

	/** 
	 * Called when a character uses this object.
	 * 
	 * @param	Character	The using character.
	 * @param	WithItem	Optional item the character used with this object (e.g. a key).
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AdvKit")
	void UsedBy(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL);
	virtual void UsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL);
	
	/**
	 * Called before UsedBy to check whether or not a character can use this object.
	 *
	 * @param	Character	The using character.
	 * @param	WithItem	Optional item the character used with this object (e.g. a key).
	 * @return	true if usable
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	bool CanBeUsedBy(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL);
	virtual bool CanBeUsedBy_Implementation(class AAdvKitCharacter* Character, class AAdvKitInventoryItem* WithItem = NULL);

};
