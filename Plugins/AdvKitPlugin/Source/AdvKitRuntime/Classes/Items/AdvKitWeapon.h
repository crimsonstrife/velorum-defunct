// Copyright 2015 Pascal Krabbe

#pragma once

#include "GameFramework/Actor.h"
#include "Items/AdvKitInventoryItem.h"
#include "Items/AdvKitProjectile.h"
#include "AdvKitWeapon.generated.h"


/**
 * @brief States a weapon can be in
 */
UENUM(BlueprintType)
enum class EAdvKitWeaponState : uint8
{
	Idle,
	Firing,
	Reloading,
};


/**
 * @brief Helper struct to handle replication of all relevant weapon properties
 */
USTRUCT(BlueprintType)
struct FAdvKitWeaponState
{
	GENERATED_USTRUCT_BODY();

	/** The current weapon state */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	EAdvKitWeaponState State;

	/** Which fire mode is currently active */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	uint8 ActiveFireMode;

	/** Set in case the weapon wants to change fire modes */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	uint8 PendingFireMode;

	/** Flag to set that the weapon wants to fire (even if it can't) */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bWantsToFire;

	/** Flag to set that weapon wants to reload as soon as possible */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bWantsToReload;

	/** Shot counter to replicate weapon effects */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	int32 BurstCounter;

	FAdvKitWeaponState()
	{
		State = EAdvKitWeaponState::Idle;
		PendingFireMode = 0;
		ActiveFireMode = 0;
		bWantsToFire = false;
		bWantsToReload = false;
		BurstCounter = 0;
	};
};


/**
 * @brief Base weapon class. Has some basic replicated handling on start-/stop firing and fire modes, as 
 * well as base methods to implement custom behaviors.
 * 
 * For this explanation the weapon is described like a gun, even though the weapon functionality
 * is not limited to guns. Calling StartFire implies that the trigger has been pulled. If the gun
 * is currently firing on an alternate mode it will stop doing that and start the new mode by calling
 * StartFireInternal. Here last checks are done, e.g. if there is enough ammunition and the weapon 
 * will be reloaded if there isn't. If all prerequisites are met the weapon will actually fire by
 * calling FireAmmunition and then start a refire timer if configured.
 *
 * StopFire works in a similar matter. You should see Start-/StopFire as intents of the player, while
 * the corresponding internal methods are the actual clockwork that govern what the weapon does.
 */
UCLASS(abstract)
class ADVKITRUNTIME_API AAdvKitWeapon : public AAdvKitInventoryItem
{
	GENERATED_BODY()

protected:

	/** Struct holding all state variables of the weapon */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing = OnRep_WeaponState)
	FAdvKitWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class USceneComponent* DefaultRootComponent;

	/*How much ammunition is held in a single magazine*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition")
	int32 AmmunitionPerMagazine;

	/*How much ammunition is left in the current magazine*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition")
	int32 AmmunitionInMagazine;

	/*How much ammunition the weapon has*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition", Replicated)
	int32 Ammunition;

	/*How much ammunition the weapon can hold*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition")
	int32 MaxAmmunition;

	/*This weapon has infinite Ammunition*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition")
	bool bHasInfiniteAmmunition;

	/** Which fire modes the weapon should have, will be instantiated on startup */
	UPROPERTY(EditDefaultsOnly, Category = "FireModes", Replicated)
	TArray< TSubclassOf<class UAdvKitFireModeConfiguration> > DefaultFireModes;

	/** List of fire modes the weapon has */
	UPROPERTY(BlueprintReadOnly, Category = "FireModes", Replicated)
	TArray<class UAdvKitFireModeConfiguration*> FireModes_OLD;

	/** Optional anim montage to play character animation when reloading. */
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	UAnimMontage* CharacterReloadMontage;

	/** How much ammunition is added with each reload. */
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	int32 AmmoByReloading;

	/** How long a reload takes. */
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	float ReloadTime;

	/** Optional texture to display on the HUD */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UTexture2D* Crosshair;

public:
	
	/** Constructor  */
	AAdvKitWeapon(const FObjectInitializer& ObjectInitializer);

	/**
	 * Tell the weapon to start firing (even though the weapon may not be able to)
	 * @param	FireModeIndex 	Which mode to start firing in
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	void StartFire(uint8 FireModeIndex = 0);
	virtual void StartFire_Implementation(uint8 FireModeIndex = 0);

	/**
	 * Stop firing, the weapon may however still continue (e.g. because it needs to calm down first)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	void StopFire();
	virtual void StopFire_Implementation();

	/**
	 * Initiates a reload
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Reload")
	void StartReload();
	virtual void StartReload_Implementation();

	/**
	 * Stops a reload in progress without reloading ammo
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Reload")
	void StopReload();
	virtual void StopReload_Implementation();

	/**
	 * Called when a shot is fired to use up ammunition.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	void ConsumeAmmunition();
	virtual void ConsumeAmmunition_Implementation();

	/**
	 * Called if player wants to but the weapon cannot fire, e.g. to check if it needs to be reloaded.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	void HandleFiringPrerequisites();
	virtual void HandleFiringPrerequisites_Implementation();

	/**
	 * Called when firing to actually shoot. Depending on the fire type of the active mode it 
	 * delegates firing to FireInstant, FireProjectile or FireCustom.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	void FireAmmunition();
	virtual void FireAmmunition_Implementation();

	/**
	 * Fires instant trace, applies damage to hit target.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	void FireInstant();
	virtual void FireInstant_Implementation();

	/**
	 * Fires a projectile configured by the current firing mode.
	 * @return The fired projectile, if successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	class AAdvKitProjectile* FireProjectile();
	virtual class AAdvKitProjectile* FireProjectile_Implementation();

	/**
	 * Custom fire hook, for other implementations.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	void FireCustom();
	virtual void FireCustom_Implementation();

protected:
	/** Handle to keep track of refiring. */
	FTimerHandle RefireTimerHandle;

public:
	/**
	 * Starts the timer to do refires
	 * @param	Duration 	How long to wait between fire calls
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	virtual void StartRefireTimer(float Duration);

	/**
	 * Stops refiring 
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	virtual void StopRefireTimer();

	/**
	 * Callback method for refire timer
	 */
	UFUNCTION()
	virtual void RefireTimer();

	/**
	 * Query if the magazine is empty
	 * @return True if empty
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammunition")
	virtual	bool IsMagazineEmpty();

	/**
	 * Check if the weapon has a given amount of ammunition (disregarding if in magazine or not)
	 * @param	Amount 	How much ammunition to ask for
	 * @return True if the weapon has at least the specified amount, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammunition")
	virtual bool HasAmmunition(int32 Amount = 1);

	/**
	 * Check if the weapon has a given amount of ammunition in the current magazine
	 * @param	Amount 	How much ammunition to ask for
	 * @return True if the weapon has at least the specified amount, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammunition")
	virtual bool HasAmmunitionInMagazine(int32 Amount = 1);


	/**
	 * Check if the weapon is currently doing nothing
	 * @return True if doing nothing
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	virtual bool IsIdle();

	/**
	 * Check if the weapon is currently firing
	 * @return True if firing
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	virtual bool IsFiring();

	/**
	 * Check if the weapon is currently reloading
	 * @return True if reloading
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|State")
	virtual bool IsReloading();

	/**
	 * Get the muzzle point of the weapon to spawn a projectile
	 * @param	FireModeIndex 	If the weapon has more than one muzzle it can return different ones
	 * @return World space location of the muzzle
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	FVector GetMuzzleLocation(uint8 FireModeIndex = 0);
	virtual FVector GetMuzzleLocation_Implementation(uint8 FireModeIndex = 0);

	/**
	 * Get the starting location for a trace for projectile/instant fire mode. The default implementation 
	 * traces from the character's viewpoint to determine the target and then fires form the muzzle location 
	 * to the hit.
	 * @param	FireModeIndex 	Depending on fire mode the weapon can have different start locations.
	 * @return World space location of where to begin the trace
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon|Fire")
	FVector GetTraceStart(uint8 FireModeIndex = 0);
	virtual FVector GetTraceStart_Implementation(uint8 FireModeIndex = 0);

	/**
	 * Gets the direction in which to fire. Adds spread to it.
	 * @param	StartFireLocation 	World space location to fire (bullet) from.
	 * @param	FireModeIndex 	Fire mode might change the direction.
	 * @return Rotation with X axis facing in direction of fire.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	virtual FRotator GetAdjustedAim(FVector StartFireLocation, uint8 FireModeIndex = 0);

	/**
	 * Add spread to a fire orientation.
	 * @param	OriginalAim 	Original fire orientation.
	 * @param	FireModeIndex 	Different fire modes can have different spreads.
	 * @return Modified aim rotation.
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Fire")
	virtual FRotator AddSpread(FRotator OriginalAim, uint8 FireModeIndex = 0);

	/**
	 * Check if the weapon can fire.
	 * @return True if it can, false if not
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Fire")
	bool CanFire();
	virtual bool CanFire_Implementation();

protected:

	/**
	 * Internal method to actually start firing the pending or current fire mode.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Fire")
	void StartFireInternal();
	virtual void StartFireInternal_Implementation();

	/**
	 * Internal method to actually stop firing the active mode.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Fire")
	void StopFireInternal();
	virtual void StopFireInternal_Implementation();

	/**
	 * Check if weapon can be reloaded.
	 * @return True if it can be reloaded.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	bool CanReload();
	virtual bool CanReload_Implementation();

	/**
	 * Internal method to start reloading timer and animations.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void StartReloadInternal();
	virtual void StartReloadInternal_Implementation();

	/**
	 * Internal method to abort reload.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void StopReloadInternal();
	virtual void StopReloadInternal_Implementation();

	/**
	 * Called when reloading succeeded. Moves ammunition from store to magazine.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void ReloadAmmunition();
	virtual void ReloadAmmunition_Implementation();

public:
	/**
	 * Adds ammunition to the weapon
	 * @param	Amount 	How much to add.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void AddAmmunition(int32 Amount);
	virtual void AddAmmunition_Implementation(int32 Amount);

protected:

	/** Handle to keep track of reloading. */
	FTimerHandle ReloadTimerHandle;

	/**
	 * Start the timer for the reloading process. When the timer fires the reload is complete.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void StartReloadTimer();
	virtual void StartReloadTimer_Implementation();

	/**
	 * Stops the reload timer (aborting it).
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void StopReloadTimer();
	virtual void StopReloadTimer_Implementation();

	/**
	 * Callback method for the reload timer.
	 */
	UFUNCTION()
	virtual void ReloadedTimer();

	/**
	 * Check if the weapon has a specified fire mode.
	 * @param	FireModeIndex 	The mode to check for
	 * @return True if mode exists, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool HasFireMode(uint8 FireModeIndex);

	/**
	 * Query a specific fire mode.
	 * @param	FireModeIndex 	The mode to get
	 * @return Pointer to the fire mode if it exists, nullptr otherwise
	 */
	virtual const UAdvKitFireModeConfiguration* GetFireMode(uint8 FireModeIndex); //TODO const for blueprint call, maybe TAssetPtr?

	/**
	 * Query a specific fire mode.
	 * @param	FireModeIndex 	The mode to get
	 * @return Pointer to the class of the fire mode if it exists, nullptr otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon", meta=(DisplayName="Get Fire Mode"))
	virtual TSubclassOf<UAdvKitFireModeConfiguration> GetFireModeClass(uint8 FireModeIndex);

	/**
	 * Query a specific fire mode.
	 * @param	FireModeIndex 	The mode to get
	 * @param[out] OutFireMode	Pointer to the fire mode if it exists, nullptr otherwise
	 * @return True if out fire mode was set, false otherwise
	 */
	virtual bool GetFireMode(uint8 FireModeIndex, const UAdvKitFireModeConfiguration** OutFireMode);

public:
	/**
	 * Weapons like swords have the ability to block damage. The parameters are from the actor's take damage event.
	 * @param	Damage 	Amount of damage taken.
	 * @param	DamageEvent 	Data package that fully describes the damage received
	 * @param	EventInstigator 	The Controller responsible for the damage
	 * @param	DamageCauser 	The Actor that directly caused the damage (e.g. the projectile that exploded, or the enemy's sword) 
	 * @return The remaining amount of damage after it's potential block.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Weapon")
	float BlockDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);
	virtual float BlockDamage_Implementation(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	/**
	 * Callback if an attack of this weapon was blocked by something
	 * @param	By 	The actor that blocked the attack
	 * @param	BlockedDamage 	How much damage was blocked
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
	void BlockedBy(AActor* By, float BlockedDamage);
	virtual void BlockedBy_Implementation(AActor* By, float BlockedDamage);

protected:
	// AActor interface
	virtual void PostInitializeComponents() override;
	// End of AActor interface


	// BEGIN Replication Block
	/**
	 * Server callback to tell the weapon to start firing (even though the weapon may not be able to)
	 * @param	FireModeIndex 	Which mode to start firing in
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Server", Server, Reliable, WithValidation)
	void StartFireServer(uint8 FireModeIndex = 0);
	virtual void StartFireServer_Implementation(uint8 FireModeIndex = 0);

	/**
	 * Server callback to stop firing, the weapon may however still continue (e.g. because it needs to calm down first)
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Server", Server, Reliable, WithValidation)
	void StopFireServer();
	virtual void StopFireServer_Implementation();

	/**
	 * Server callback to initiate a reload
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Server", Server, Reliable, WithValidation)
	void StartReloadServer();
	virtual void StartReloadServer_Implementation();

	/**
	 * Server callback to stop a reload in progress without reloading ammo
	 */
	UFUNCTION(BlueprintCallable, Category = "Weapon|Server", Server, Reliable, WithValidation)
	void StopReloadServer();
	virtual void StopReloadServer_Implementation();

	/**
	 * Callback method when the server state changes
	 * @param OldState	State before the change
	 */
	UFUNCTION()
	virtual void OnRep_WeaponState(FAdvKitWeaponState OldState);
	// END Replication Block


public:

	//State Events
	/**
	 * Callback event when the weapon starts firing.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Fire")
	void OnFireBegin();
	virtual void OnFireBegin_Implementation();

	/**
	 * Callback event when FireAmmunition was called.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Fire")
	void OnFireShot();
	virtual void OnFireShot_Implementation();

	/**
	 * Callback event when firing stops.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Fire")
	void OnFireEnd();
	virtual void OnFireEnd_Implementation();

	/**
	 * Callback event when the weapon starts to reload.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void OnReloadBegin();
	virtual void OnReloadBegin_Implementation();

	/**
	 * Callback event when the weapon has finished reloading.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void OnReloaded();
	void OnReloaded_Implementation();

	/**
	 * Callback event when reloading was aborted.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Reload")
	void OnReloadEnd();
	virtual void OnReloadEnd_Implementation();

	/**
	 * Callback event when the weapon wants to fire but there is no ammo left in the current magazine.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Ammunition")
	void OnMagazineEmpty();
	virtual void OnMagazineEmpty_Implementation();

	/**
	 * Callback event when the weapon wants to fire but has no mor ammunition.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon Events|Ammunition")
	void OnWeaponEmpty();
	virtual void OnWeaponEmpty_Implementation();
};
