// Copyright 2015 Pascal Krabbe

#include "AdvKitEditor.h"
#include "AdvKitTransitionComponentPointVisualizer.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"
#include "Environment/AdvKitZone.h"
#include "Player/AdvKitCharacter.h"
#include "ActorPickerMode.h"
#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "AdvKitTransitionComponentPointVisualizer" 

IMPLEMENT_HIT_PROXY(HTransitionPointVisProxy, HComponentVisProxy)
IMPLEMENT_HIT_PROXY(HTransitionPointPositionProxy, HTransitionPointVisProxy)

class FAdvKitTransitionComponentPointVisualizerCommands : public TCommands < FAdvKitTransitionComponentPointVisualizerCommands >
{
public:
	FAdvKitTransitionComponentPointVisualizerCommands() : TCommands <FAdvKitTransitionComponentPointVisualizerCommands>
		(
			"AdvKitTransitionComponentPointVisualizer",
			LOCTEXT("AdvKitTransitionComponentPointVisualizer", "Transition Component Point Visualizer"),
			NAME_None,
			FEditorStyle::GetStyleSetName()
			) {}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(PickTargetZone, "Pick transition target", "Set a new target zone for the transition.", EUserInterfaceActionType::Button, FInputGesture());
	}

public:
	/** Change Target Zone */
	TSharedPtr<FUICommandInfo> PickTargetZone;

};

void FAdvKitTransitionComponentPointVisualizer::OnRegister()
{
	FAdvKitTransitionComponentPointVisualizerCommands::Register();
	VisualizerActions = MakeShareable(new FUICommandList);
	const auto& Commands = FAdvKitTransitionComponentPointVisualizerCommands::Get();

	VisualizerActions->MapAction(
		Commands.PickTargetZone,
		FExecuteAction::CreateSP(this, &FAdvKitTransitionComponentPointVisualizer::OnPickTargetZone));
}

void FAdvKitTransitionComponentPointVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	auto VisComponent = Cast<const UAdvKitTransitionComponentPoint>(Component);
	if (!VisComponent)
	{
		return;
	}

	bool bSelected = (Component == GetEditedTransitionComponent());

	//Query all necessary debug info
	const FVector LineStart = VisComponent->GetSourceLocation();
	const FVector LineEnd = VisComponent->GetTargetLocation();
	FColor LineColor = VisComponent->bIsDynamic ? FColorList::MandarianOrange : FColorList::Grey;
	if (bSelected)
	{
		FLinearColor Temp(LineColor);
		Temp *= 2.0f;
		LineColor = Temp.ToFColor(false);
	}
	const float LineThickness = 2;

	//Arrow lines
	const float	ArrowLength = 8.0f;// FMath::Min<float>(16.0f, FVector::Dist(LineStart, LineEnd)*0.25f);
	const FVector Forward = (LineEnd - LineStart).GetSafeNormal();
	FVector Right, Up;
	Forward.FindBestAxisVectors(Right, Up);

	//Draw lines and arrow head
	PDI->SetHitProxy(new HTransitionPointVisProxy(Component));
	DrawArrow(Component, View, PDI, LineStart, LineEnd, LineColor);

	if (bSelected)
	{
		DrawArrow(Component, View, PDI, LineStart, LineStart + Component->GetOwner()->GetTransform().TransformVector(VisComponent->TransitionDirection) * 25, FColorList::Grey);

		DrawPosition(Component, View, PDI, LineStart, ETransitionPointPosition::Source, LineColor);
		DrawPosition(Component,View,PDI, LineEnd, ETransitionPointPosition::Target, LineColor);
	}

	PDI->SetHitProxy(nullptr);

}


bool FAdvKitTransitionComponentPointVisualizer::VisProxyHandleClick(FLevelEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click)
{
	if (VisProxy && VisProxy->Component.IsValid())
	{
		auto* TransitionComp = CastChecked<const UAdvKitTransitionComponentPoint>(VisProxy->Component.Get());

		TransitionCompName = TransitionComp->GetName();

		if (TransitionCompName != "")
		{
			AActor* OldTransitionOwningActor = TransitionOwningActor.Get();
			TransitionOwningActor = TransitionComp->GetOwner();
			if (OldTransitionOwningActor != TransitionOwningActor)
			{
				// Reset selection state if we are selecting a different actor to the one previously selected
				//ChangeSelectionState(INDEX_NONE, false);
				//SelectedSegmentIndex = INDEX_NONE;
				//SelectedTangentHandle = INDEX_NONE;
				//SelectedTangentHandleType = ESelectedTangentHandle::None;

				SelectedPosition = ETransitionPointPosition::None;
			}

			if (VisProxy->IsA(HTransitionPointPositionProxy::StaticGetType()))
			{
				HTransitionPointPositionProxy* Proxy = (HTransitionPointPositionProxy*)VisProxy;
				SelectedPosition = Proxy->Position;
				return true;
			}
			else
			{
				SelectedPosition = ETransitionPointPosition::None;
			}

			return true;
		}
	}

	return false;
}

bool FAdvKitTransitionComponentPointVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocaction) const
{
	auto EditedTransition = GetEditedTransitionComponent();
	if (!IsValid(EditedTransition))
	{
		return false;
	}

	switch (SelectedPosition)
	{
	case ETransitionPointPosition::None:
		return false;
	case ETransitionPointPosition::Source:
		OutLocaction = EditedTransition->GetSourceLocation();
		return true;
	case ETransitionPointPosition::Target:
		OutLocaction = EditedTransition->GetTargetLocation();
		return true;
	}

	return false;
}

bool FAdvKitTransitionComponentPointVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
{
	auto EditedTransition = GetEditedTransitionComponent();
	if (!IsValid(EditedTransition))
	{
		return false;
	}

	auto SourceZone = Cast<AAdvKitZone>(EditedTransition->GetOwner());
	auto TargetZone = EditedTransition->TargetZone.Get();
	auto CharacterClass = EditedTransition->UseableBy;

	switch (SelectedPosition)
	{
	case ETransitionPointPosition::None:
		return false;
	case ETransitionPointPosition::Source:
		EditedTransition->SourcePosition.Set(
			EditedTransition->SourcePosition.Base,
			SourceZone->ConstrainPositionToZone_Implementation(
				*EditedTransition->SourcePosition + DeltaTranslate,
				GetCharacterHalfExtent(CharacterClass, SourceZone),
				SourceZone->GetActorRotation()
				));
		return true;
	case ETransitionPointPosition::Target:
		if (TargetZone)
		{
			EditedTransition->TargetPosition.Set(
				EditedTransition->TargetPosition.Base,
				TargetZone->ConstrainPositionToZone_Implementation(
					*EditedTransition->TargetPosition + DeltaTranslate,
					GetCharacterHalfExtent(CharacterClass, TargetZone),
					TargetZone->GetActorRotation()
					));
		}
		else
		{
			EditedTransition->TargetPosition.Set(
				EditedTransition->TargetPosition.Base,
				*EditedTransition->TargetPosition + DeltaTranslate);
		}
		return true;
	}

	return true;
}

bool FAdvKitTransitionComponentPointVisualizer::HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::Escape)
	{
		CancelPickTarget();
		return true;
	}
	//if (Key == EKeys::LeftShift)
	//{
	//	if (Event == EInputEvent::IE_Pressed)
	//	{
	//		return true;
	//	}
	//	else if (Event == EInputEvent::IE_Released)
	//	{
	//		return true;
	//	}
	//}

	return false;
}

TSharedPtr<SWidget> FAdvKitTransitionComponentPointVisualizer::GenerateContextMenu() const
{
	FMenuBuilder MenuBuilder(true, VisualizerActions);
	{
		MenuBuilder.BeginSection("Transition Actions");
		{
			MenuBuilder.AddMenuEntry(FAdvKitTransitionComponentPointVisualizerCommands::Get().PickTargetZone);
		}
		MenuBuilder.EndSection();
	}

	TSharedPtr<SWidget> MenuWidget = MenuBuilder.MakeWidget();
	return MenuWidget;
}

void FAdvKitTransitionComponentPointVisualizer::EndEditing()
{
	TransitionOwningActor = nullptr;
	SelectedPosition = ETransitionPointPosition::None;
	TransitionCompName = "";
}

UAdvKitTransitionComponentPoint* FAdvKitTransitionComponentPointVisualizer::GetEditedTransitionComponent() const
{
	if (!TransitionOwningActor.IsValid())
	{
		return nullptr;
	}

	auto Components = TransitionOwningActor->GetComponentsByClass(UAdvKitTransitionComponentPoint::StaticClass());
	for (auto Component : Components)
	{
		if (Component->GetName() == TransitionCompName)
		{
			return Cast<UAdvKitTransitionComponentPoint>(Component);
		}
	}

	return nullptr;
}


void FAdvKitTransitionComponentPointVisualizer::DrawArrow(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Source, const FVector& Target, const FColor& Color)
{
	static const float	ArrowLength = 8.0f;
	static const float LineThickness = 2;

	const FVector Forward = (Target - Source).GetSafeNormal();
	FVector Right, Up;
	Forward.FindBestAxisVectors(Right, Up);

	//Draw lines and arrow head
	PDI->DrawLine(Source, Target, Color, SDPG_World, LineThickness);
	PDI->DrawLine(Target, Target + (-Forward + Right).GetSafeNormal()*ArrowLength, Color, SDPG_World, LineThickness);
	PDI->DrawLine(Target, Target + (-Forward - Right).GetSafeNormal()*ArrowLength, Color, SDPG_World, LineThickness);
	PDI->DrawLine(Target, Target + (-Forward + Up).GetSafeNormal()*ArrowLength, Color, SDPG_World, LineThickness);
	PDI->DrawLine(Target, Target + (-Forward - Up).GetSafeNormal()*ArrowLength, Color, SDPG_World, LineThickness);
}

void FAdvKitTransitionComponentPointVisualizer::DrawPosition(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Location, const ETransitionPointPosition& Position, const FColor& Color)
{
	PDI->SetHitProxy(new HTransitionPointPositionProxy(Component, Position));
	PDI->DrawPoint(Location, Color, 20.f, SDPG_Foreground);
}

void FAdvKitTransitionComponentPointVisualizer::OnPickTargetZone()
{
	FActorPickerModeModule& ActorPickerMode = FModuleManager::Get().GetModuleChecked<FActorPickerModeModule>("ActorPickerMode");

	if (ActorPickerMode.IsInActorPickingMode())
	{
		ActorPickerMode.EndActorPickingMode();
	}
	else
	{
		ActorPickerMode.BeginActorPickingMode(
			FOnGetAllowedClasses::CreateSP(this, &FAdvKitTransitionComponentPointVisualizer::OnGetAllowedClasses),
			FOnShouldFilterActor::CreateSP(this, &FAdvKitTransitionComponentPointVisualizer::IsFilteredActor),
			FOnActorSelected::CreateSP(this, &FAdvKitTransitionComponentPointVisualizer::OnActorSelected)
			);
	}
}

void FAdvKitTransitionComponentPointVisualizer::CancelPickTarget()
{
	FActorPickerModeModule& ActorPickerMode = FModuleManager::Get().GetModuleChecked<FActorPickerModeModule>("ActorPickerMode");
	ActorPickerMode.EndActorPickingMode();
}

void FAdvKitTransitionComponentPointVisualizer::OnGetAllowedClasses(TArray<const UClass*>& AllowedClasses)
{
	AllowedClasses.Add(AAdvKitZone::StaticClass());
}

void FAdvKitTransitionComponentPointVisualizer::OnActorSelected(AActor* InActor)
{
	auto NewZone = Cast<AAdvKitZone>(InActor);
	if (!NewZone)
	{
		return;
	}

	auto EditedTransition = GetEditedTransitionComponent();
	if (!EditedTransition)
	{
		return;
	}


	FVector NewTargetLocation = NewZone->ConstrainPositionToZone_Implementation(EditedTransition->GetTargetLocation(), GetCharacterHalfExtent(EditedTransition->UseableBy, NewZone));
	EditedTransition->Init(EditedTransition->UseableBy,
		EditedTransition->GetSourceLocation(),
		NewTargetLocation,
		EditedTransition->TargetPhysics,
		NewZone,
		EditedTransition->TargetCustomPhysics);
}

bool FAdvKitTransitionComponentPointVisualizer::IsFilteredActor(const AActor* const Actor) const
{
	return Actor != TransitionOwningActor.Get();
}

const FVector FAdvKitTransitionComponentPointVisualizer::GetCharacterHalfExtent(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) const
{
	if (!ForCharacterClass || !ForZone)
	{
		return FVector::ZeroVector;
	}

	//Not sure if this can ever happen, but I am a pessimist :P
	AAdvKitCharacter* DefaultCharacter = ForCharacterClass.GetDefaultObject();
	if (!DefaultCharacter)
	{
		return FVector::ZeroVector;
	}
	auto DummyMoveComp = DefaultCharacter->GetAdvMovementComponent();
	check(DummyMoveComp);

	return DummyMoveComp->GetHalfExtentForZone(ForZone);
}
#undef LOCTEXT_NAMESPACE