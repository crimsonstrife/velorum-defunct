// Copyright 2015 Pascal Krabbe

#pragma once

#include "ComponentVisualizer.h"

class UAdvKitTransitionComponentPoint;
class AAdvKitCharacter;
class AAdvKitZone;

enum class ETransitionPointPosition : uint8
{
	None,
	Source,
	Target
};

struct HTransitionPointVisProxy : public HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HTransitionPointVisProxy(const UActorComponent* InComponent)
		: HComponentVisProxy(InComponent, HPP_Wireframe)
	{}
};

struct HTransitionPointPositionProxy : public HTransitionPointVisProxy
{
	DECLARE_HIT_PROXY();

	HTransitionPointPositionProxy(const UActorComponent* InComponent, ETransitionPointPosition Position)
		: HTransitionPointVisProxy(InComponent)
		, Position(Position)
	{}

	ETransitionPointPosition Position;
};


/**
 * @brief Editor Visualizer class for UAdvKitTransitionComponentPoint
 */
class FAdvKitTransitionComponentPointVisualizer: public FComponentVisualizer
{
public:
	/**
	 * Constructor
	 */
	FAdvKitTransitionComponentPointVisualizer() {};;

	/**
	 * Destructor
	 */
	virtual ~FAdvKitTransitionComponentPointVisualizer() {};;
 
	// Begin FComponentVisualizer interface
	virtual void OnRegister() override;
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual bool VisProxyHandleClick(FLevelEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click) override;
	virtual bool GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	virtual bool HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale) override;
	virtual bool HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) override;
	virtual TSharedPtr<SWidget> GenerateContextMenu() const override;
	virtual void EndEditing() override;
	// End FComponentVisualizer interface

	/** Get the transition component we are currently editing */
	UAdvKitTransitionComponentPoint* GetEditedTransitionComponent() const;
	void DrawArrow(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Source, const FVector& Target, const FColor& Color);
	void DrawPosition(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Location, const ETransitionPointPosition& Position, const FColor& Color);

	/**Context menu commands*/
	TSharedPtr<FUICommandList> VisualizerActions;

	/** Actor that owns the currently edited transition */
	TWeakObjectPtr<AActor> TransitionOwningActor;

	/** Name of property on the actor that references the transition we are editing */
	FString TransitionCompName;

	ETransitionPointPosition SelectedPosition;

	UFUNCTION()
	void OnPickTargetZone();
	void CancelPickTarget();

	void OnGetAllowedClasses(TArray<const UClass*>& AllowedClasses);
	void OnActorSelected(AActor* InActor);
	bool IsFilteredActor(const AActor* const Actor) const;

	/**
	* Utility method to query a character's size properties.
	* @param	ForCharacterClass 	Character to get size of.
	* @param	ForZone 	Zone to get character's size in.
	* @return Half extent (Radius, Radius, Half Height) of character
	*/
	virtual const FVector GetCharacterHalfExtent(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) const;
};