// Copyright 2015 Pascal Krabbe

#include "AdvKitEditor.h"
#include "AdvKitTraceComponentVisualizer.h"
#include "Player/AdvKitTraceUtilityComponent.h"

FAdvKitTraceComponentVisualizer::FAdvKitTraceComponentVisualizer()
{
}

FAdvKitTraceComponentVisualizer::~FAdvKitTraceComponentVisualizer()
{
}

void FAdvKitTraceComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	const UAdvKitTraceUtilityComponent* AdvKitTraceUtilityComponent = Cast<const UAdvKitTraceUtilityComponent>(Component);
	//Draw only for the trace utility component
	if (!AdvKitTraceUtilityComponent)
	{
		return;
	}

	//Query all necessary debug info
	const FColor LineColor = AdvKitTraceUtilityComponent->GetDebugLineColor();
	const float LineLength = AdvKitTraceUtilityComponent->GetTraceLength();
	const float LineThickness = AdvKitTraceUtilityComponent->GetDebugLineThickness();
	const float	ArrowLength = FMath::Min<float>(16.0f, LineLength*0.25f);

	//Values for line
	const FVector Forward = AdvKitTraceUtilityComponent->GetForwardVector();
	const FVector Right = AdvKitTraceUtilityComponent->GetRightVector();
	const FVector LineStart = AdvKitTraceUtilityComponent->GetComponentLocation();
	const FVector LineEnd = LineStart + Forward * LineLength;

	//Draw lines and arrow head
	PDI->DrawLine(LineStart, LineEnd, LineColor, SDPG_World, LineThickness);
	PDI->DrawLine(LineEnd, LineEnd + (-Forward + Right).GetSafeNormal()*ArrowLength, LineColor, SDPG_World, LineThickness);
	PDI->DrawLine(LineEnd, LineEnd + (-Forward - Right).GetSafeNormal()*ArrowLength, LineColor, SDPG_World, LineThickness);
}