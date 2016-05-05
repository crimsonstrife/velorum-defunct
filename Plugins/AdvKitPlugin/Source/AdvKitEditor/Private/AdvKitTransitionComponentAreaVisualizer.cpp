// Copyright 2015 Pascal Krabbe

#include "AdvKitEditor.h"
#include "AdvKitTransitionComponentAreaVisualizer.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"
#include "Environment/AdvKitZone.h"
#include "Player/AdvKitCharacter.h"
#include "ActorPickerMode.h"
#include "UnrealEd.h"

#define LOCTEXT_NAMESPACE "AdvKitTransitionComponentAreaVisualizer" 

IMPLEMENT_HIT_PROXY(HTransitionAreaVisProxy, HComponentVisProxy)
IMPLEMENT_HIT_PROXY(HTransitionAreaPositionProxy, HTransitionAreaVisProxy)

class FAdvKitTransitionComponentAreaVisualizerCommands : public TCommands < FAdvKitTransitionComponentAreaVisualizerCommands >
{
public:
	FAdvKitTransitionComponentAreaVisualizerCommands() : TCommands <FAdvKitTransitionComponentAreaVisualizerCommands>
		(
			"AdvKitTransitionComponentAreaVisualizer",
			LOCTEXT("AdvKitTransitionComponentAreaVisualizer", "Transition Component Area Visualizer"),
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

void FAdvKitTransitionComponentAreaVisualizer::OnRegister()
{
	FAdvKitTransitionComponentAreaVisualizerCommands::Register();
	VisualizerActions = MakeShareable(new FUICommandList);
	const auto& Commands = FAdvKitTransitionComponentAreaVisualizerCommands::Get();

	VisualizerActions->MapAction(
		Commands.PickTargetZone,
		FExecuteAction::CreateSP(this, &FAdvKitTransitionComponentAreaVisualizer::OnPickTargetZone));
}


void FAdvKitTransitionComponentAreaVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	static const float ArrowDistanceUnits = 100.0f;

	auto VisComponent = Cast<const UAdvKitTransitionComponentArea>(Component);
	if (!VisComponent)
	{
		return;
	}

	bool bSelected = (Component == GetEditedTransitionComponent());

	const FVector MinWorldSource = VisComponent->GetMinSourceLocation();
	const FVector MaxWorldSource = VisComponent->GetMaxSourceLocation();
	const FVector MinWorldTarget = VisComponent->GetMinTargetLocation();
	const FVector MaxWorldTarget = VisComponent->GetMaxTargetLocation();

	FColor LineColor = VisComponent->bIsDynamic ? FColorList::MandarianOrange : FColorList::Grey;
	if (bSelected)
	{
		FLinearColor Temp(LineColor);
		Temp *= 2.0f;
		LineColor = Temp.ToFColor(false);
	}


	const float Distance = FVector::Dist(MinWorldSource, MaxWorldSource);
	float ArrowCount = FPlatformMath::CeilToInt(Distance / ArrowDistanceUnits);
	float Step = 1.0f / ArrowCount;

	PDI->SetHitProxy(new HTransitionAreaVisProxy(Component));
	for (float Alpha = 0; Alpha <= 1; Alpha += Step)
	{
		FVector LerpSource = FMath::Lerp<FVector>(MinWorldSource, MaxWorldSource, Alpha);
		FVector LerpTarget = FMath::Lerp<FVector>(MinWorldTarget, MaxWorldTarget, Alpha);
		DrawArrow(Component, View, PDI, LerpSource, LerpTarget, LineColor);
	}
	DrawArrow(Component, View, PDI, MaxWorldSource, MaxWorldTarget, LineColor);

	if (bSelected)
	{
		FVector Center = FMath::Lerp<FVector>(MinWorldSource, MaxWorldSource, 0.5f);
		DrawArrow(Component, View, PDI, Center, Center + Component->GetOwner()->GetTransform().TransformVector(VisComponent->TransitionDirection) * 25, FColorList::Grey);

		DrawPosition(Component, View, PDI, MinWorldSource, ETransitionAreaPosition::MinSource, LineColor);
		DrawPosition(Component, View, PDI, MaxWorldSource, ETransitionAreaPosition::MaxSource, LineColor);
		DrawPosition(Component, View, PDI, MinWorldTarget, ETransitionAreaPosition::MinTarget, LineColor);
		DrawPosition(Component, View, PDI, MaxWorldTarget, ETransitionAreaPosition::MaxTarget, LineColor);
	}

	PDI->SetHitProxy(nullptr);
}

bool FAdvKitTransitionComponentAreaVisualizer::VisProxyHandleClick(FLevelEditorViewportClient* InViewportClient, HComponentVisProxy* VisProxy, const FViewportClick& Click)
{
	if (VisProxy && VisProxy->Component.IsValid())
	{
		auto* TransitionComp = CastChecked<const UAdvKitTransitionComponentArea>(VisProxy->Component.Get());

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

				SelectedPosition = ETransitionAreaPosition::None;
			}

			if (VisProxy->IsA(HTransitionAreaPositionProxy::StaticGetType()))
			{
				HTransitionAreaPositionProxy* Proxy = (HTransitionAreaPositionProxy*)VisProxy;
				SelectedPosition = Proxy->Position;
				return true;
			}
			else
			{
				SelectedPosition = ETransitionAreaPosition::None;
			}

			return true;
		}
	}

	return false;
}

bool FAdvKitTransitionComponentAreaVisualizer::GetWidgetLocation(const FEditorViewportClient* ViewportClient, FVector& OutLocaction) const
{
	auto EditedTransition = GetEditedTransitionComponent();
	if (!IsValid(EditedTransition))
	{
		return false;
	}

	switch (SelectedPosition)
	{
	case ETransitionAreaPosition::None:
		return false;
	case ETransitionAreaPosition::MinSource:
		OutLocaction = EditedTransition->GetMinSourceLocation();
		return true;
	case ETransitionAreaPosition::MaxSource:
		OutLocaction = EditedTransition->GetMaxSourceLocation();
		return true;
	case ETransitionAreaPosition::MinTarget:
		OutLocaction = EditedTransition->GetMinTargetLocation();
		return true;
	case ETransitionAreaPosition::MaxTarget:
		OutLocaction = EditedTransition->GetMaxTargetLocation();
		return true;
	}

	return false;
}

bool FAdvKitTransitionComponentAreaVisualizer::HandleInputDelta(FEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltaRotate, FVector& DeltaScale)
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
	case ETransitionAreaPosition::None:
		return false;
	case ETransitionAreaPosition::MinSource:
		EditedTransition->MinSourcePosition.Set(
			EditedTransition->MinSourcePosition.Base, 
			SourceZone->ConstrainPositionToZone_Implementation(
				*EditedTransition->MinSourcePosition + DeltaTranslate,
				GetCharacterHalfExtent(CharacterClass,SourceZone),
				SourceZone->GetActorRotation()
				));
		return true;
	case ETransitionAreaPosition::MaxSource:
		EditedTransition->MaxSourcePosition.Set(
			EditedTransition->MaxSourcePosition.Base,
			SourceZone->ConstrainPositionToZone_Implementation(
				*EditedTransition->MaxSourcePosition + DeltaTranslate,
				GetCharacterHalfExtent(CharacterClass, SourceZone),
				SourceZone->GetActorRotation()
				));
		return true;
	case ETransitionAreaPosition::MinTarget:
		if (TargetZone)
		{
			EditedTransition->MinTargetPosition.Set(
				EditedTransition->MinTargetPosition.Base,
				TargetZone->ConstrainPositionToZone_Implementation(
					*EditedTransition->MinTargetPosition + DeltaTranslate,
					GetCharacterHalfExtent(CharacterClass, TargetZone),
					TargetZone->GetActorRotation()
					));
		}
		else
		{
			EditedTransition->MinTargetPosition.Set(
				EditedTransition->MinTargetPosition.Base,
				*EditedTransition->MinTargetPosition + DeltaTranslate);
		}
		return true;
	case ETransitionAreaPosition::MaxTarget:
		if (TargetZone)
		{
			EditedTransition->MaxTargetPosition.Set(
			EditedTransition->MaxTargetPosition.Base,
				TargetZone->ConstrainPositionToZone_Implementation(
				*EditedTransition->MaxTargetPosition + DeltaTranslate,
				GetCharacterHalfExtent(CharacterClass, TargetZone),
				TargetZone->GetActorRotation()
				));
		}
		else
		{
			EditedTransition->MaxTargetPosition.Set(
				EditedTransition->MaxTargetPosition.Base,
				*EditedTransition->MaxTargetPosition + DeltaTranslate);
		}
		return true;
	}

	return true;
}

bool FAdvKitTransitionComponentAreaVisualizer::HandleInputKey(FEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event)
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

TSharedPtr<SWidget> FAdvKitTransitionComponentAreaVisualizer::GenerateContextMenu() const
{
	FMenuBuilder MenuBuilder(true, VisualizerActions);
	{
		MenuBuilder.BeginSection("Transition Actions");
		{
			MenuBuilder.AddMenuEntry(FAdvKitTransitionComponentAreaVisualizerCommands::Get().PickTargetZone);
		}
		MenuBuilder.EndSection();
	}

	TSharedPtr<SWidget> MenuWidget = MenuBuilder.MakeWidget();
	return MenuWidget;
}

void FAdvKitTransitionComponentAreaVisualizer::EndEditing()
{
	TransitionOwningActor = nullptr;
	SelectedPosition = ETransitionAreaPosition::None;
	TransitionCompName = "";
}

UAdvKitTransitionComponentArea* FAdvKitTransitionComponentAreaVisualizer::GetEditedTransitionComponent() const
{
	if (!TransitionOwningActor.IsValid())
	{
		return nullptr;
	}

	auto Components = TransitionOwningActor->GetComponentsByClass(UAdvKitTransitionComponentArea::StaticClass());
	for (auto Component : Components)
	{
		if (Component->GetName() == TransitionCompName)
		{
return Cast<UAdvKitTransitionComponentArea>(Component);
		}
	}

	return nullptr;
}


void FAdvKitTransitionComponentAreaVisualizer::DrawArrow(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Source, const FVector& Target, const FColor& Color)
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

void FAdvKitTransitionComponentAreaVisualizer::DrawPosition(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI, const FVector& Location, const ETransitionAreaPosition& Position, const FColor& Color)
{
	PDI->SetHitProxy(new HTransitionAreaPositionProxy(Component, Position));
	PDI->DrawPoint(Location, Color, 20.f, SDPG_Foreground);
}

void FAdvKitTransitionComponentAreaVisualizer::OnPickTargetZone()
{
	FActorPickerModeModule& ActorPickerMode = FModuleManager::Get().GetModuleChecked<FActorPickerModeModule>("ActorPickerMode");

	if (ActorPickerMode.IsInActorPickingMode())
	{
		ActorPickerMode.EndActorPickingMode();
	}
	else
	{
		ActorPickerMode.BeginActorPickingMode(
			FOnGetAllowedClasses::CreateSP(this, &FAdvKitTransitionComponentAreaVisualizer::OnGetAllowedClasses),
			FOnShouldFilterActor::CreateSP(this, &FAdvKitTransitionComponentAreaVisualizer::IsFilteredActor),
			FOnActorSelected::CreateSP(this, &FAdvKitTransitionComponentAreaVisualizer::OnActorSelected)
			);
	}
}

void FAdvKitTransitionComponentAreaVisualizer::CancelPickTarget()
{
	FActorPickerModeModule& ActorPickerMode = FModuleManager::Get().GetModuleChecked<FActorPickerModeModule>("ActorPickerMode");
	ActorPickerMode.EndActorPickingMode();
}

void FAdvKitTransitionComponentAreaVisualizer::OnGetAllowedClasses(TArray<const UClass*>& AllowedClasses)
{
	AllowedClasses.Add(AAdvKitZone::StaticClass());
}

void FAdvKitTransitionComponentAreaVisualizer::OnActorSelected(AActor* InActor)
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


	FVector NewMinTargetLocation = NewZone->ConstrainPositionToZone_Implementation(EditedTransition->GetMinTargetLocation(), GetCharacterHalfExtent(EditedTransition->UseableBy, NewZone));
	FVector NewMaxTargetLocation = NewZone->ConstrainPositionToZone_Implementation(EditedTransition->GetMaxTargetLocation(), GetCharacterHalfExtent(EditedTransition->UseableBy, NewZone));

	EditedTransition->Init(EditedTransition->UseableBy,
		EditedTransition->GetMinSourceLocation(),
		NewMinTargetLocation,
		EditedTransition->GetMaxSourceLocation(),
		NewMaxTargetLocation,
		EditedTransition->TargetPhysics,
		NewZone,
		EditedTransition->TargetCustomPhysics);
}

bool FAdvKitTransitionComponentAreaVisualizer::IsFilteredActor(const AActor* const Actor) const
{
	return Actor != TransitionOwningActor.Get();
}

const FVector FAdvKitTransitionComponentAreaVisualizer::GetCharacterHalfExtent(TSubclassOf<AAdvKitCharacter> ForCharacterClass, AAdvKitZone* ForZone) const
{
	if(!ForCharacterClass || !ForZone)
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