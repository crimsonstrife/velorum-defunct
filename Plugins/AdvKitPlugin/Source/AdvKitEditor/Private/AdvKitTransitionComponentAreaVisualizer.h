// Copyright 2015 Pascal Krabbe

#pragma once

#include "ComponentVisualizer.h"


enum class ETransitionAreaPosition : uint8
{
	None,
	MinSource,
	MaxSource,
	MinTarget,
	MaxTarget
};

class UAdvKitTransitionComponentArea;
class AAdvKitCharacter;
class AAdvKitZone;

struct HTransitionAreaVisProxy : public HComponentVisProxy
{
	DECLARE_HIT_PROXY();

	HTransitionAreaVisProxy(const UActorComponent* InComponent)
		: HComponentVisProxy(InComponent, HPP_Wireframe)
	{}
};

struct HTransitionAreaPositionProxy : public HTransitionAreaVisProxy
{
	DECLARE_HIT_PROXY();

	HTransitionAreaPositionProxy(const UActorComponent* InComponent, ETransitionAreaPosition Position)
		: HTransitionAreaVisProxy(InComponent)
		, Position(Position)
	{}

	ETransitionAreaPosition Position;
};

/**
 * @brief Editor Visualizer class for UAdvKitTransitionComponentArea
 */
class FAdvKitTransitionComponentAreaVisualizer: public FComponentVisualizer
{
public:
	/**
	 * Constructor
	 */
	FAdvKitTransitionComponentAreaVisualizer() {};

	/**
	 * Destructor
	 */
	virtual ~FAdvKitTransitionComponentAreaVisualizer() {};
 
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
	UAdvKitTransitionComponentArea* GetEditedTransitionComponent() const;
	void DrawArrow(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Source, const FVector& Target, const FColor& Color);
	void DrawPosition(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Location, const ETransitionAreaPosition& Position, const FColor& Color);

	/**Context menu commands*/
	TSharedPtr<FUICommandList> VisualizerActions;

	/** Actor that owns the currently edited transition */
	TWeakObjectPtr<AActor> TransitionOwningActor;

	/** Name of property on the actor that references the transition we are editing */
	FString TransitionCompName;

	ETransitionAreaPosition SelectedPosition;

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