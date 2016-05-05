// Copyright 2015 Pascal Krabbe

#pragma once

#include "AdvKitTypes.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AdvKitTraceUtilityComponent.generated.h"

/**
 * @brief A Utility component to make traces with that are configurable from the editor.
 *
 * This component will use it's transformation to do a trace along its X-Axis with the 
 * length defined by its TraceLength property.
 */
UCLASS(ClassGroup = Utility, hidecategories = (Mobility, LOD, Events, Collision, Physics, Rendering, "Rendering | Material", TextureStreaming, Input, Lighting, Base, Activation), meta = (BlueprintSpawnableComponent))
class ADVKITRUNTIME_API UAdvKitTraceUtilityComponent : public USceneComponent
{
	GENERATED_BODY()

protected:
	/** Trace Length for this component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit")
	float TraceLength;

	/** Color when rendering the debug line for the trace*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit")
	FColor DebugLineColor;

	/** Thickness when rendering the debug line for the trace*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvKit")
	float DebugLineThickness;

public:

	/**
	 * Constructor
	 */
	UAdvKitTraceUtilityComponent(const FObjectInitializer& ObjectInitializer);

	/**
	 * Set the length of the trace
	 * @param	NewLength 	New trace length
	 */
	virtual void SetTraceLength(float NewLength);

	/**
	 * @brief Get the length of the trace 
	 * @return Length of the trace line
	 */
	virtual float GetTraceLength() const;

	/**
	 * Set the color when drawing a debug line
	 * @param	NewColor 	New color for debug draw
	 */
	virtual void SetDebugLineColor(FColor NewColor);

	/**
	 * @brief Get the color when drawing a debug line 
	 * @return Debug draw color
	 */
	virtual FColor GetDebugLineColor() const;

	/**
	 * Set the thickness when drawing a debug line
	 * @param	NewThickness 	Put argument desc here
	 */
	virtual void SetDebugLineThickness(float NewThickness);

	/**
	 * @brief Get the thickness when drawing a debug line 
	 * @return Debug draw thickness
	 */
	virtual float GetDebugLineThickness() const;

	/**
	 * Get the location of the trace start in world space
	 * @return Trace start location in world space
	 */
	UFUNCTION(BlueprintCallable, Category = "Trace")
	virtual FVector GetStart();

	/**
	 * Get the location of the trace end in world space
	 * @return Trace end location in world space
	 */
	UFUNCTION(BlueprintCallable, Category = "Trace")
	virtual FVector GetEnd();

	/**
	 * Draw a debug line to visualize the trace
	 */
	UFUNCTION(BlueprintCallable, Category = "Debug")
	virtual void DrawDebug();

	/**
	 * Trace a ray against the world and return if a blocking hit is found.
	 * @param  TraceChannel    The 'channel' that this ray is in, used to determine which components to hit
	 * @param  Params          Additional parameters used for the trace
	 * @param  ResponseParam	ResponseContainer to be used for this trace
	 * @return TRUE if a blocking hit is found
	 */
	bool LineTraceTest(ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);

	/**
	 * Trace a ray against the world and return if a blocking hit is found.
	 * @param  Params          	Additional parameters used for the trace
	 * @param  ObjectQueryParams	List of object types it's looking for
	 * @return TRUE if any hit is found
	 */
	bool LineTraceTest(const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams);

	/**
	 * Trace a ray against the world and return the first blocking hit
	 * @param  OutHit          First blocking hit found
	 * @param  TraceChannel    The 'channel' that this ray is in, used to determine which components to hit
	 * @param  Params          Additional parameters used for the trace
	 * @param  ResponseParam	ResponseContainer to be used for this trace
	 * @return TRUE if a blocking hit is found
	 */
	bool LineTraceSingle(struct FHitResult& OutHit, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);

	/**
	 * Trace a ray against the world and return the first blocking hit
	 * @param  OutHit          First blocking hit found
	 * @param  Params          	Additional parameters used for the trace
	 * @param  ObjectQueryParams	List of object types it's looking for
	 * @return TRUE if any hit is found
	 */
	bool LineTraceSingle(struct FHitResult& OutHit, const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams);

	/**
	 * Trace a ray against the world and return overlapping hits and then first blocking hit
	 * Results are sorted, so a blocking hit (if found) will be the last element of the array
	 * Only the single closest blocking result will be generated, no tests will be done after that
	 * @param  OutHits         Array of hits found between ray and the world
	 * @param  TraceChannel    The 'channel' that this ray is in, used to determine which components to hit
	 * @param  Params          Additional parameters used for the trace
	 * @param  ResponseParam	ResponseContainer to be used for this trace
	 * @return TRUE if OutHits contains any blocking hit entries
	 */
	bool LineTraceMulti(TArray<struct FHitResult>& OutHits, ECollisionChannel TraceChannel, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);

	/**
	 * Trace a ray against the world and return overlapping hits and then first blocking hit
	 * Results are sorted, so a blocking hit (if found) will be the last element of the array
	 * Only the single closest blocking result will be generated, no tests will be done after that
	 * @param  OutHits         Array of hits found between ray and the world
	 * @param  Params          	Additional parameters used for the trace
	 * @param  ObjectQueryParams	List of object types it's looking for
	 * @return TRUE if any hit is found
	 */
	bool LineTraceMulti(TArray<struct FHitResult>& OutHits, const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams);

	/**
	 * Sweep a sphere against the world and return if a blocking hit is found.
	 * @param  TraceChannel    The 'channel' that this trace uses, used to determine which components to hit
	 * @param	CollisionShape		CollisionShape - supports Box, Sphere, Capsule
	 * @param  Params          Additional parameters used for the trace
	 * 	@param 	ResponseParam	ResponseContainer to be used for this trace	 	 
	 * @return TRUE if a blocking hit is found
	 */
	bool SweepTest(const FQuat& Rot, ECollisionChannel TraceChannel, const struct FCollisionShape & CollisionShape, const struct FCollisionQueryParams& Params, const struct FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);

	/**
	 * Sweep a sphere against the world and return if a blocking hit is found.
	 * @param	CollisionShape		CollisionShape - supports Box, Sphere, Capsule
	 * @param  Params          	Additional parameters used for the trace
	 *	@param	ObjectQueryParams	List of object types it's looking for
	 * @return TRUE if any hit is found
	 */
	bool SweepTest(const FQuat& Rot, const struct FCollisionShape & CollisionShape, const struct FCollisionQueryParams& Params, const struct FCollisionObjectQueryParams& ObjectQueryParams);


	/**
	 * Sweep the geometry of the supplied component, and determine the set of components that it hits
	 * @param  OutHits         Array of hits found between ray and the world
	 * @param  PrimComp        Component to use geometry from to test against the world. Transform of this component is ignored
	 * @param  Rot             Rotation to place PrimComp geometry at to test against the world (rotation remains constant over trace)
	 * @param  Params          Additional parameters used for the trace
	 * @return TRUE if OutHits contains any blocking hit entries
	 */
	bool ComponentSweepMulti(TArray<struct FHitResult>& OutHits, class UPrimitiveComponent* PrimComp, const FRotator& Rot, const struct FComponentQueryParams& Params);


	/**
	* Does a collision trace along the given line and returns the first blocking hit encountered.
	* This trace finds the objects that RESPONDS to the given TraceChannel
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param TraceChannel
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "LineTraceByChannel", Keywords = "raycast"))
	bool LineTraceSingleBlueprint(ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);

	/**
	* Does a collision trace along the given line and returns all hits encountered up to and including the first blocking hit.
	* This trace finds the objects that RESPOND to the given TraceChannel
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param TraceChannel	The channel to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a blocking hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "MultiLineTraceByChannel", Keywords = "raycast"))
		bool LineTraceMultiBlueprint(ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);

	/**
	* Sweeps a sphere along the given line and returns the first blocking hit encountered.
	* This trace finds the objects that RESPONDS to the given TraceChannel
	*
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Radius		Radius of the sphere to sweep
	* @param TraceChannel
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "SphereTraceByChannel", Keywords = "sweep"))
		bool SphereTraceSingleBlueprint(float Radius, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);

	/**
	* Sweeps a sphere along the given line and returns all hits encountered up to and including the first blocking hit.
	* This trace finds the objects that RESPOND to the given TraceChannel
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Radius		Radius of the sphere to sweep
	* @param TraceChannel
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHits		A list of hits, sorted along the trace from start to finish.  The blocking hit will be the last hit, if there was one.
	* @return				True if there was a blocking hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "MultiSphereTraceByChannel", Keywords = "sweep"))
		bool SphereTraceMultiBlueprint(float Radius, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);

	/**
	* Sweeps a box along the given line and returns the first blocking hit encountered.
	* This trace finds the objects that RESPONDS to the given TraceChannel
	*
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param HalfSize	    Distance from the center of box along each axis
	* @param Orientation	Orientation of the box
	* @param TraceChannel
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit			Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "BoxTraceByChannel", Keywords = "sweep"))
		bool BoxTraceSingleBlueprint(const FVector HalfSize, const FRotator Orientation, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);

	/**
	* Sweeps a box along the given line and returns all hits encountered.
	* This trace finds the objects that RESPONDS to the given TraceChannel
	*
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param HalfSize	    Distance from the center of box along each axis
	* @param Orientation	Orientation of the box
	* @param TraceChannel
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHits		A list of hits, sorted along the trace from start to finish. The blocking hit will be the last hit, if there was one.
	* @return				True if there was a blocking hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "MultiBoxTraceByChannel", Keywords = "sweep"))
		bool BoxTraceMultiBlueprint(FVector HalfSize, const FRotator Orientation, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);


	/**
	* Sweeps a capsule along the given line and returns the first blocking hit encountered.
	* This trace finds the objects that RESPOND to the given TraceChannel
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Radius		Radius of the capsule to sweep
	* @param HalfHeight	Distance from center of capsule to tip of hemisphere endcap.
	* @param TraceChannel
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "CapsuleTraceByChannel", Keywords = "sweep"))
		bool CapsuleTraceSingleBlueprint(float Radius, float HalfHeight, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);

	/**
	* Sweeps a capsule along the given line and returns all hits encountered up to and including the first blocking hit.
	* This trace finds the objects that RESPOND to the given TraceChannel
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Radius		Radius of the capsule to sweep
	* @param HalfHeight	Distance from center of capsule to tip of hemisphere endcap.
	* @param TraceChannel
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHits		A list of hits, sorted along the trace from start to finish.  The blocking hit will be the last hit, if there was one.
	* @return				True if there was a blocking hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "MultiCapsuleTraceByChannel", Keywords = "sweep"))
		bool CapsuleTraceMultiBlueprint(float Radius, float HalfHeight, ETraceTypeQuery TraceChannel, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);

	/**
	* Does a collision trace along the given line and returns the first hit encountered.
	* This only finds objects that are of a type specified by ObjectTypes.
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param ObjectTypes	Array of Object Types to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "LineTraceForObjects", Keywords = "raycast"))
		bool LineTraceSingleForObjectBlueprints(const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);

	/**
	* Does a collision trace along the given line and returns all hits encountered.
	* This only finds objects that are of a type specified by ObjectTypes.
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param ObjectTypes	Array of Object Types to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "MultiLineTraceForObjects", Keywords = "raycast"))
		bool LineTraceMultiForObjectsBlueprint(const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);

	/**
	* Sweeps a sphere along the given line and returns the first hit encountered.
	* This only finds objects that are of a type specified by ObjectTypes.
	*
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Radius		Radius of the sphere to sweep
	* @param ObjectTypes	Array of Object Types to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "SphereTraceForObjects", Keywords = "sweep"))
		bool SphereTraceSingleForObjectBlueprints(float Radius, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);

	/**
	* Sweeps a sphere along the given line and returns all hits encountered.
	* This only finds objects that are of a type specified by ObjectTypes.
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Radius		Radius of the sphere to sweep
	* @param ObjectTypes	Array of Object Types to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHits		A list of hits, sorted along the trace from start to finish.  The blocking hit will be the last hit, if there was one.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true",AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "MultiSphereTraceForObjects", Keywords = "sweep"))
		bool SphereTraceMultiForObjectsBlueprint(float Radius, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);


	/**
	* Sweeps a box along the given line and returns the first hit encountered.
	* This only finds objects that are of a type specified by ObjectTypes.
	*
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Orientation
	* @param HalfSize		Radius of the sphere to sweep
	* @param ObjectTypes	Array of Object Types to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit			Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "BoxTraceForObjects", Keywords = "sweep"))
		bool BoxTraceSingleForObjectsBlueprint(const FVector HalfSize, const FRotator Orientation, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);


	/**
	* Sweeps a box along the given line and returns all hits encountered.
	* This only finds objects that are of a type specified by ObjectTypes.
	*
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Orientation
	* @param HalfSize		Radius of the sphere to sweep
	* @param ObjectTypes	Array of Object Types to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHits		A list of hits, sorted along the trace from start to finish.  The blocking hit will be the last hit, if there was one.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "MultiBoxTraceForObjects", Keywords = "sweep"))
		bool BoxTraceMultiForObjectsBlueprint(const FVector HalfSize, const FRotator Orientation, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);

	/**
	* Sweeps a capsule along the given line and returns the first hit encountered.
	* This only finds objects that are of a type specified by ObjectTypes.
	*
	* @param WorldContext	World context
	* @param Start			Start of line segment.
	* @param End			End of line segment.
	* @param Radius		Radius of the capsule to sweep
	* @param HalfHeight	Distance from center of capsule to tip of hemisphere endcap.
	* @param ObjectTypes	Array of Object Types to trace
	* @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	* @param OutHit		Properties of the trace hit.
	* @return				True if there was a hit, false otherwise.
	*/
	UFUNCTION(BlueprintCallable, Category = "Trace", meta = (bIgnoreSelf = "true", AutoCreateRefTerm = "ActorsToIgnore", DisplayName = "CapsuleTraceForObjects", Keywords = "sweep"))
	bool CapsuleTraceSingleForObjectsBlueprint(float Radius, float HalfHeight, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, bool bIgnoreSelf);


	/**
	 * Sweeps a capsule along the given line and returns all hits encountered.
	 * This only finds objects that are of a type specified by ObjectTypes.
	 * 
	 * @param WorldContext	World context
	 * @param Start			Start of line segment.
	 * @param End			End of line segment.
	 * @param Radius		Radius of the capsule to sweep
	 * @param HalfHeight	Distance from center of capsule to tip of hemisphere endcap.
	 * @param ObjectTypes	Array of Object Types to trace 
	 * @param bTraceComplex	True to test against complex collision, false to test against simplified collision.
	 * @param OutHits		A list of hits, sorted along the trace from start to finish.  The blocking hit will be the last hit, if there was one.
	 * @return				True if there was a hit, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Collision", meta=(bIgnoreSelf="true", AutoCreateRefTerm="ActorsToIgnore", DisplayName = "MultiCapsuleTraceForObjects", Keywords="sweep"))
	bool CapsuleTraceMultiForObjects(float Radius, float HalfHeight, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, EDrawDebugTrace::Type DrawDebugType, TArray<FHitResult>& OutHits, bool bIgnoreSelf);

};
