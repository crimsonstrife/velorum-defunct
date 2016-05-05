// Copyright 2015 Pascal Krabbe

#pragma once

#include "AdvKitWeapon.h"
#include "AdvKitWeaponTracing.generated.h"

/**
 * @brief Base class for sword like weapons. 
 *
 * This weapon class has a defined line that will hurt anything that intersects it. 
 * When the weapon is moved it will try to hit anything that crossed the path of the 
 * trace line during the move.
 */
UCLASS()
class ADVKITRUNTIME_API AAdvKitWeaponTracing : public AAdvKitWeapon
{
	GENERATED_BODY()

protected:
	/** Component responsible for handling the tracing. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	class UAdvKitTraceUtilityComponent* TraceComponent;

	/** Flag to enable tracing updates */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsTracing;

	/** Helper array to collect hit actors in order to avoid double hits during trace updates */
	TMap<AActor*, FHitResult> DamageStack;

	/** Cached location of the last trace start before the weapon moved */
	FVector LastTraceStartPosition;
	/** Cached location of the last trace end before the weapon moved */
	FVector LastTraceEndPosition;
	
	/** Cached location of the last trace component location before the weapon moved */
	FVector LastTraceComponentLocation;
	/** Cached location of the last trace component rotation before the weapon moved */
	FQuat LastTraceComponentRotation;

	/** The number of sub traces when the weapon moved is Distance/UnitsPerTrace */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float UnitsPerTrace;

public:
	/**
	 * Constructor
	 */
	AAdvKitWeaponTracing(const FObjectInitializer& ObjectInitializer);

	/**
	 * Enable tracing updates.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void StartTracing();

	/**
	 * Disable tracing updates.
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvKit")
	virtual void StopTracing();

	//Begin AActor Interface
	virtual void Tick(float DeltaSeconds) override;
	//End AActor Interface

	/**
	 * Callback when the weapon hit anything.
	 * @param	Hits 	List of hits
	 */
	void OnTraceHits(const TArray<FHitResult>& Hits);


	/**
	 * Callback when the weapon hit anything. Called for each hit.
	 * @param	Hit 	The hit of the trace
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AdvKit")
	void OnTraceHit(FHitResult Hit);
	virtual void OnTraceHit_Implementation(FHitResult Hit);

	/**
	 * Tries to apply damage to a trace hit. This can fail if the hit actor was
	 * already damaged during the update pass.
	 * @param	Hit 	The hit to apply damage to
	 * @return	True if damage was done
	 */
	virtual bool TryToApplyDamageToHit(FHitResult& Hit);
};
