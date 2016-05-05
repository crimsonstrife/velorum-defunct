// Copyright 2015 Pascal Krabbe

#pragma once

#include "ComponentVisualizer.h"

/**
 * @brief Editor Visualizer class for @see UAdvKitTraceUtilityComponent
 */
class FAdvKitTraceComponentVisualizer: public FComponentVisualizer
{
public:
	/**
	 * Constructor
	 */
	FAdvKitTraceComponentVisualizer();

	/**
	 * Destructor
	 */
	virtual ~FAdvKitTraceComponentVisualizer();
 
	// Begin FComponentVisualizer interface
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	// End FComponentVisualizer interface
};