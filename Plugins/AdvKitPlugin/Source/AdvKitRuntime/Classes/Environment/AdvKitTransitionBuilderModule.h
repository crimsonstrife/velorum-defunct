// Copyright 2015 Pascal Krabbe

#pragma once

#include "Object.h"
#include "AdvKitTransitionBuilderModule.generated.h"

class AAdvKitZone;
class AAdvKitCharacter;
class UAdvKitTransitionComponentArea;
class UAdvKitTransitionComponentPoint;

/**
 * @brief Base class for modules that create transition. A module takes care of creating transitions for
 * specific types of zones. 
 */
UCLASS(Blueprintable, EditInlineNew, abstract, CollapseCategories)
class ADVKITRUNTIME_API UAdvKitTransitionBuilderModule : public UObject
{
	GENERATED_BODY()

protected:

	/** When checking if two positions align, this is the maximum variance for positions to be considered equal. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transitions")
	float ErrorMarginPosition;

	/** When checking if two directions align, this is the maximum variance of their dot product to be considered equal. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transitions")
	float ErrorMarginDot;

	/** During the build process this is used to keep track of zones that transitions where created to or removed from */
	UPROPERTY(BlueprintReadWrite)
	TArray<AAdvKitZone*> AffectedZones;

	/** During the build process this is used to keep track transitions that where created or updated */
	UPROPERTY(BlueprintReadWrite)
	TArray<UAdvKitTransitionComponent*> CreatedOrUpdatedTransitions;

public:
	/** For which character classes this builder should create transitions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
	TArray<TSubclassOf<AAdvKitCharacter>> DefaultCharacterClasses;

public:

	/**
	 * Constructor
	 */
	UAdvKitTransitionBuilderModule(const FObjectInitializer& ObjectInitializer);

	/**
	 * Create transitions for zone 
	 * @param[out]	OutAffectedZones	All zones that are targets of the created transitions
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	void CreateTransitions(TArray<AAdvKitZone*>& OutAffectedZones);
	virtual void CreateTransitions_Implementation(TArray<AAdvKitZone*>& OutAffectedZones);

protected:

	/**
	 * @return The zone this build module belongs to
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	AAdvKitZone* GetOuterZone();

	/**
	 * Check if this module can create transitions for a given zone and character.
	 * @param	ForCharacterClass 	Character to create transitions for.
	 * @param	ForZone 	Zone to create transitions for.
	 * @return True if this module can create transitions for the given arguments.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	bool CanCreateTransitionsFor(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone);
	virtual bool CanCreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone);

	/**
	 * Create transitions for a given character and zone
	 * @param	ForCharacterClass 	Character to create transitions for.
	 * @param	ForZone 	Zone to create transitions for.
	 * @param[out]	OutAffectedZones	All zones that are targets of the created transitions
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	void CreateTransitionsFor(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone);
	virtual void CreateTransitionsFor_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone);

	/**
	 * Queries all zones that might be potential targets for new transitions.
	 * @param	ForCharacterClass 	Character to create transitions for.
	 * @param	ForZone 	Zone to create transitions for.
	 * @param[out]	OutZones	All zones that are potential targets.
	 * @return	True if any potential targets were found, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	bool GatherPotentialTargetZones(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones);
	virtual bool GatherPotentialTargetZones_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone, TArray<AAdvKitZone*>& OutZones);
	
	/**
	 * Creates transitions between two given zones
	 * @param	ForCharacterClass 	Character to create transitions for.
	 * @param	SourceZone	Zone to create transitions from.
	 * @param	TargetZone	Zone to create transitions to.
	 * @return	True if any transitions were created.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "AdvKit")
	bool CreateTransitionBetween(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone);
	virtual bool CreateTransitionBetween_Implementation(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone);

	/**
	 * Utility method to query a character's size properties. 
	 * @param	ForCharacterClass 	Character to get size of.
	 * @param	ForZone 	Zone to get character's size in.
	 * @return Half extent (Radius, Radius, Half Height) of character
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual const FVector GetCharacterHalfExtent(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) const;

	/**
	 * Utility method to get default movement component of a given character
	 * @param	ForCharacterClass 	Character to get movement properties of.
	 * @return The character's movement component.
	 */
	//UFUNCTION(BlueprintCallable, Category = "AdvKit", meta =(DisplayName="GetCharacterMovementComponent"))
	virtual const UAdvKitCharacterMovementComponent* GetCharacterMovementComponent(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const;

	/**
	 * Utility method to get movement properties of a given character
	 * @param	ForCharacterClass 	Character to get movement properties of.
	 * @return The character's movement properties.
	 */
	virtual const FAdvKitMovementProperties* GetCharacterMovementProperties(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const;

	/**
	 * Utility method to get movement properties of a given character
	 * @param	ForCharacterClass 	Character to get movement properties of.
	 * @return The character's movement properties.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit", meta =(DisplayName="GetCharacterMovementProperties"))
	const FAdvKitMovementProperties& GetCharacterMovementProperties_BP(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const;

	/**
	 * Utility method to get the default object for a given character class.
	 * @param	ForCharacterClass 	Character class to get default object of.
	 * @return Default object of given class.
	 */
	virtual AAdvKitCharacter* GetDefaultCharacterObject(TSubclassOf<AAdvKitCharacter> ForCharacterClass) const;

	/**
	 * Does an overlap check to find all zones within it.
	 * @param	ToZone 	The origin zone to find overlaps for.
	 * @param	OverlapStart 	Start of the overlap.
	 * @param	OverlapEnd 		End of the overlap.
	 * @param	OverlapRadius	Radius of the overlap.
	 * @param[out]	OutOtherZones 	All zones that were overlapped.
	 * @return True if any zones were overlapped, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual bool OverlapForCloseZones(AAdvKitZone* ToZone, FVector OverlapStart, FVector OverlapEnd, float OverlapRadius, TArray<AAdvKitZone*>& OutOtherZones);


	virtual TArray<class UAdvKitTransitionComponent*> GetPreExistingTransitions(FString Id, AAdvKitZone* FromZone, AAdvKitZone* ToZone, TSubclassOf<AAdvKitCharacter> ForCharacterClass) const;
	virtual TArray<class UAdvKitTransitionComponent*> GetPreExistingTransitions(AAdvKitZone* FromZone, AAdvKitZone* ToZone) const;
	virtual TArray<class UAdvKitTransitionComponent*> GetPreExistingTransitions(AAdvKitZone* FromZone) const;

	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	UAdvKitTransitionComponentPoint* CreateTransitionPoint(FString Id, AAdvKitZone* SourceZone, const FVector& WorldSourceLocation, const FVector& WorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, AAdvKitZone* TargetZone = nullptr, uint8 CustomMovementMode = 0);
	
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	UAdvKitTransitionComponentArea* CreateTransitionArea(FString Id, AAdvKitZone* SourceZone, const FVector& MinWorldSourceLocation, const FVector& MinWorldTargetLocation, const FVector& MaxWorldSourceLocation, const FVector& MaxWorldTargetLocation, TSubclassOf<AAdvKitCharacter> ForCharacter, EAdvKitMovementMode MovementMode, AAdvKitZone* TargetZone = nullptr, uint8 CustomMovementMode = 0);

	FString MakeUniqueId(FString BaseId, AAdvKitZone* SourceZone, AAdvKitZone* TargetZone);
};
