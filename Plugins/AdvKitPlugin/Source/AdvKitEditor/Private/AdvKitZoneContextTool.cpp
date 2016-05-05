// Copyright 2015 Pascal Krabbe

#include "AdvKitEditor.h"
#include "Player/AdvKitCharacter.h"
#include "Environment/AdvKitZone.h"
#include "Environment/Zones/AdvKitZoneLine.h"
#include "Environment/Zones/AdvKitZoneRectangle.h"
#include "Environment/AdvKitTransitionBuilderModule.h"

#define LOCTEXT_NAMESPACE "AdvKitEditorZoneContextTool"

void RebuildSelectedTransitions(TArray<AAdvKitZone*> SelectedZones);

void AddSupportedCharacterClass(TArray<AAdvKitZone*> SelectedZones, UClass* CharacterClass)
{
	for (auto Zone : SelectedZones)
	{
		if (!IsValid(Zone))
		{
			continue;
		}

		Zone->AddCharacterClass(CharacterClass);
	}

	RebuildSelectedTransitions(SelectedZones);
}

void SetSupportedCharacterClass(TArray<AAdvKitZone*> SelectedZones, UClass* CharacterClass)
{
	for (auto Zone : SelectedZones)
	{
		if (!IsValid(Zone))
		{
			continue;
		}

		Zone->SetCharacterClass(CharacterClass);
	}

	RebuildSelectedTransitions(SelectedZones);
}


void RemoveSupportedCharacterClass(TArray<AAdvKitZone*> SelectedZones, UClass* CharacterClass)
{
	for (auto Zone : SelectedZones)
	{
		if (!IsValid(Zone))
		{
			continue;
		}

		Zone->RemoveCharacterClass(CharacterClass);
	}

	RebuildSelectedTransitions(SelectedZones);
}

void RebuildSelectedTransitions(TArray<AAdvKitZone*> SelectedZones)
{
	for (auto Zone : SelectedZones)
	{
		if (!IsValid(Zone))
		{
			continue;
		}

		Zone->RerunConstructionScripts();
	}
}

void AddTransitionArea(TArray<AAdvKitZone*> SelectedZones)
{
	for (auto Zone : SelectedZones)
	{
		if (!IsValid(Zone))
		{
			continue;
		}

		auto LineZone = Cast<AAdvKitZoneLine>(Zone);
		if (LineZone)
		{
			FVector Start = Zone->ConstrainPositionToZone_Implementation(LineZone->GetZoneStartWorld(), FVector(42.0f, 42.0f, 96.0f), Zone->GetActorRotation());
			FVector End = Zone->ConstrainPositionToZone_Implementation(LineZone->GetZoneEndWorld(), FVector(42.0f, 42.0f, 96.0f), Zone->GetActorRotation());
			Zone->RegisterTransitionArea(Start, Start + FVector::UpVector * 100, End, End + FVector::UpVector * 100, AAdvKitCharacter::StaticClass(), Zone->GetPhysics(), nullptr, Zone->GetCustomPhysics());
			continue;
		}

		auto RectZone = Cast<AAdvKitZoneRectangle>(Zone);
		if (RectZone)
		{
			FVector MinRect = RectZone->LocalRectMinBoundary;
			FVector MaxRect = RectZone->LocalRectMaxBoundary;

			float CenterZ = FMath::Lerp<float>(MinRect.Z, MaxRect.Z, 0.5f);

			FVector Start = RectZone->GetTransform().TransformPosition(FVector(MinRect.X, MinRect.Y, CenterZ));
			Start = Zone->ConstrainPositionToZone_Implementation(Start, FVector(42.0f, 42.0f, 96.0f), Zone->GetActorRotation());
			
			FVector End = RectZone->GetTransform().TransformPosition(FVector(MinRect.X, MaxRect.Y, CenterZ));
			End = Zone->ConstrainPositionToZone_Implementation(End, FVector(42.0f, 42.0f, 96.0f), Zone->GetActorRotation());

			Zone->RegisterTransitionArea(Start, Start + FVector::UpVector * 100, End, End + FVector::UpVector * 100, AAdvKitCharacter::StaticClass(), Zone->GetPhysics(), nullptr, Zone->GetCustomPhysics());
		}
	}
}

void AddTransitionPoint(TArray<AAdvKitZone*> SelectedZones)
{
	for (auto Zone : SelectedZones)
	{
		if (!IsValid(Zone))
		{
			continue;
		}

		auto LineZone = Cast<AAdvKitZoneLine>(Zone);
		if (LineZone)
		{
			FVector Center = FMath::Lerp<FVector>(LineZone->GetZoneStartWorld(), LineZone->GetZoneEndWorld(),0.5f);
			Center = Zone->ConstrainPositionToZone_Implementation(Center, FVector(42.0f, 42.0f, 96.0f), Zone->GetActorRotation());
			Zone->RegisterTransition(Center, Center + FVector::UpVector * 100, AAdvKitCharacter::StaticClass(), Zone->GetPhysics(), nullptr, Zone->GetCustomPhysics());
			continue;
		}

		auto RectZone = Cast<AAdvKitZoneRectangle>(Zone);
		if (RectZone)
		{
			FVector Center = FMath::Lerp<FVector>(RectZone->GetZoneRectMinWorld(), RectZone->GetZoneRectMaxWorld(), 0.5f);
			Center = Zone->ConstrainPositionToZone_Implementation(Center, FVector(42.0f, 42.0f, 96.0f), Zone->GetActorRotation());
			Zone->RegisterTransition(Center, Center + FVector::UpVector * 100, AAdvKitCharacter::StaticClass(), Zone->GetPhysics(), nullptr, Zone->GetCustomPhysics());
		}
	}
}

void RebuildAllTransitions()
{
	if (!GEditor)
	{
		UE_LOG(LogTemp, Error, TEXT("No GEditor!"));
		return;
	}

	auto World = GEditor->GetEditorWorldContext().World();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("No World!"));
		return;
	}

	uint32 ZoneCount = 0;
	for (TActorIterator<AAdvKitZone> ZoneItr(World); ZoneItr; ++ZoneItr)
	{
		auto Zone = *ZoneItr;
		if (!Zone)
		{
			continue;
		}

		ZoneCount++;
		Zone->GenerateTransitions_Implementation(true); //TODO finde out why calling GenerateTransitions does nothing
	}

	UE_LOG(LogTemp, Log, TEXT("Rebuild transitions for %d zones."), ZoneCount);
}

void FillAddCharacterClassMenu(FMenuBuilder& MenuBuilder, TArray<AAdvKitZone*> SelectedZones)
{
	TArray<UClass*> CharacterClasses;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(AAdvKitCharacter::StaticClass()))// && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			CharacterClasses.Add(*It);
		}
	}

	for (auto CharacterClass : CharacterClasses)
	{
		if (CharacterClass->GetName().StartsWith("SKEL_") 
			|| CharacterClass->GetName().StartsWith("REINST_"))
		{
			continue;
		}

		FString ClassName = CharacterClass->GetName();
		ClassName.RemoveFromEnd("_C",ESearchCase::CaseSensitive);

		FUIAction UIAction;
		UIAction.ExecuteAction.BindStatic(&AddSupportedCharacterClass, SelectedZones, CharacterClass);
		MenuBuilder.AddMenuEntry(
			FText::FromString(ClassName),
			FText::FromString(ClassName),
			FSlateIcon(), UIAction);
	}
}

void FillSetCharacterClassMenu(FMenuBuilder& MenuBuilder, TArray<AAdvKitZone*> SelectedZones)
{
	TArray<UClass*> CharacterClasses;

	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(AAdvKitCharacter::StaticClass()))// && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			CharacterClasses.Add(*It);
		}
	}

	for (auto CharacterClass : CharacterClasses)
	{
		if (CharacterClass->GetName().StartsWith("SKEL_")
			|| CharacterClass->GetName().StartsWith("REINST_"))
		{
			continue;
		}

		FString ClassName = CharacterClass->GetName();
		ClassName.RemoveFromEnd("_C", ESearchCase::CaseSensitive);

		FUIAction UIAction;
		UIAction.ExecuteAction.BindStatic(&SetSupportedCharacterClass, SelectedZones, CharacterClass);
		MenuBuilder.AddMenuEntry(
			FText::FromString(ClassName),
			FText::FromString(ClassName),
			FSlateIcon(), UIAction);
	}
}

void FillRemoveCharacterClassMenu(FMenuBuilder& MenuBuilder, TArray<AAdvKitZone*> SelectedZones)
{
	TArray<UClass*> CharacterClasses;

	for (auto Zone : SelectedZones)
	{
		if (!IsValid(Zone))
		{
			continue;
		}

		for (auto Module : Zone->BuildModules)
		{
			if (!IsValid(Module))
			{
				continue;
			}

			for (auto Class : Module->DefaultCharacterClasses)
			{
				CharacterClasses.AddUnique(Class);
			}
		}
	}

	//for (TObjectIterator<UClass> It; It; ++It)
	//{
	//	if (It->IsChildOf(AAdvKitCharacter::StaticClass()))// && !It->HasAnyClassFlags(CLASS_Abstract))
	//	{
	//		CharacterClasses.Add(*It);
	//	}
	//}

	for (auto CharacterClass : CharacterClasses)
	{
		FString ClassName = CharacterClass->GetName();
		ClassName.RemoveFromEnd("_C", ESearchCase::CaseSensitive);

		FUIAction UIAction;
		UIAction.ExecuteAction.BindStatic(&RemoveSupportedCharacterClass, SelectedZones, CharacterClass);
		MenuBuilder.AddMenuEntry(
			FText::FromString(ClassName),
			FText::FromString(ClassName),
			FSlateIcon(), UIAction);
	}
}

void AddAdvKitContextMenuCommands(FMenuBuilder& MenuBuilder, TArray<AAdvKitZone*> SelectedZones)
{
	if (SelectedZones.Num() == 0)
	{
		return;
	}

	MenuBuilder.BeginSection("Adventure Kit Context", FText::FromString("Adventure Kit"));
	{
		FUIAction UIActionAddTransitionPoint;
		UIActionAddTransitionPoint.ExecuteAction.BindStatic(&AddTransitionPoint, SelectedZones);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("Add Transition Point", "Add Transition Point"),
			LOCTEXT("Add Transition Point Tooltip", "Adds a transition point to the zone."),
			FSlateIcon(), UIActionAddTransitionPoint);

		FUIAction UIActionAddTransitionArea;
		UIActionAddTransitionArea.ExecuteAction.BindStatic(&AddTransitionArea, SelectedZones);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("Add Transition Area", "Add Transition Area"),
			LOCTEXT("Add Transition Area Tooltip", "Adds a transition area to the zone."),
			FSlateIcon(), UIActionAddTransitionArea);

		FUIAction UIActionRebuildSelectedTransitions;
		UIActionRebuildSelectedTransitions.ExecuteAction.BindStatic(&RebuildSelectedTransitions, SelectedZones);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("Rebuild Transitions", "Rebuild Transitions"),
			LOCTEXT("Rebuild Transitions Tooltip", "Rebuild transitions of selected zones."),
			FSlateIcon(), UIActionRebuildSelectedTransitions);

		FUIAction UIActionRebuildAllTransitions;
		UIActionRebuildAllTransitions.ExecuteAction.BindStatic(&RebuildAllTransitions);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("Rebuild All Transitions", "Rebuild All Transitions"),
			LOCTEXT("Rebuild All Transitions Tooltip", "Rebuild transitions of all zones in the level."),
			FSlateIcon(), UIActionRebuildAllTransitions);

		MenuBuilder.AddSubMenu(
			LOCTEXT("Add Character Class", "Add Transition Character"),
			LOCTEXT("Add Character Class Tooltip", "Adds a character class for all transition builders."),
			FNewMenuDelegate::CreateStatic(&FillAddCharacterClassMenu, SelectedZones)
			);

		MenuBuilder.AddSubMenu(
			LOCTEXT("Set Character Class", "Set Transition Character"),
			LOCTEXT("Set Character Class Tooltip", "Sets a character class for all transition builders and removes all others."),
			FNewMenuDelegate::CreateStatic(&FillSetCharacterClassMenu, SelectedZones)
			);

		
		MenuBuilder.AddSubMenu(
			LOCTEXT("Remove Character Class", "Remove Transition Character"),
			LOCTEXT("Remove Character Class Tooltip", "Removes a character class from all transition builders."),
			FNewMenuDelegate::CreateStatic(&FillRemoveCharacterClassMenu, SelectedZones)
			);
	}
	MenuBuilder.EndSection();
}

/**
* Extends the level viewport context menu with adventure kit-specific menu items
*/
TSharedRef<FExtender> ExtendLevelViewportContextMenuForAdvKit(const TSharedRef<FUICommandList> CommandList, TArray<AActor*> SelectedActors)
{
	TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);

	TArray<AAdvKitZone*> SelectedZones;
	for (auto Actor : SelectedActors)
	{
		auto Zone = Cast<AAdvKitZone>(Actor);
		if (!IsValid(Zone))
		{
			continue;
		}

		SelectedZones.Add(Zone);
	}

	if (SelectedZones.Num() == 0)
	{
		return Extender.ToSharedRef();
	}

	Extender->AddMenuExtension("ActorControl", EExtensionHook::Before, CommandList,
		FMenuExtensionDelegate::CreateStatic(&AddAdvKitContextMenuCommands, SelectedZones));

	return Extender.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE
