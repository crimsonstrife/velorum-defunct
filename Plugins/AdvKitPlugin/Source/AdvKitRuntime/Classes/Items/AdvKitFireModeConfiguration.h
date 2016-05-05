// Copyright 2015 Pascal Krabbe

#pragma once

#include "UObject.h"
#include "AdvKitFireModeConfiguration.generated.h"

/** @brief Types of firing that are handled */
UENUM()
enum class EAdvKitRifleFireType : uint8
{
	InstantHit,
	Projectile,
	Custom
};

/**
 * @brief Configuration to hold values for firing @see AAdvKitWeapon.
 */
UCLASS(Blueprintable)
class ADVKITRUNTIME_API UAdvKitFireModeConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * Constructor
	 */
	UAdvKitFireModeConfiguration(const FObjectInitializer& ObjectInitializer);

	/*How much ammunition is consumed per shot*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition")
	int32 AmmunitionPerShot;

	/*How much to spread (in degrees) when shooting*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammunition")
	float Spread;

	/*Only fire for this mode when the trigger is released*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	bool bFireOnStop;

	/*Does this mode want to fire, even if it cannot yet?*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	bool bWantsToFire;

	/*Can this weapon refire?*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (EditCondition = "!bFireOnStop"))
	bool bShouldRefire;

	/*How long to wait between firing (in Seconds)*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", meta = (EditCondition = "!bFireOnStop"))
	float RefireTime;

	/** Montage to play on the character that is firing the weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	UAnimMontage* CharacterFireMontage;

	/** Montage to play on the character that is firing the weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Fire | Projectile")
	TSubclassOf<class AAdvKitProjectile> ProjectileClass;

	/** Trace length for projectile and instant hit mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire | Instant")
	float TraceDistance;

	/*How much damage is caused per instant hit*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire | Instant")
	float InstantDamage;

	/** The type of damage to apply for instant hits */
	UPROPERTY(EditDefaultsOnly, Category = "Fire | Instant")
	TSubclassOf<class UDamageType> InstantDamageTypeClass;

	/** Which fire type this mode is */
	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	EAdvKitRifleFireType FireType;

#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const override;
#endif
};
