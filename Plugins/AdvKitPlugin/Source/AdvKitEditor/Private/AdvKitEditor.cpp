// Copyright 2015 Pascal Krabbe

#include "AdvKitEditor.h"
#include "SlateStyle.h"
#include "LevelEditor.h"

#include "AdvKitTraceComponentVisualizer.h"
#include "Player/AdvKitTraceUtilityComponent.h"

#include "AdvKitTransitionComponentPointVisualizer.h"
#include "Environment/Transitions/AdvKitTransitionComponentPoint.h"

#include "AdvKitTransitionComponentAreaVisualizer.h"
#include "Environment/Transitions/AdvKitTransitionComponentArea.h"

#define LOCTEXT_NAMESPACE "AdvKitEditor"

/**
 * @brief Editor module to register custom visualizer for UAdvKitTraceUtilityComponent.
 */
class FAdvKitEditor : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	template<typename TVis, typename TComp> void RegisterVisualizer();

	TSharedPtr<FUICommandList> CommandList;

	/** Handle to a registered LevelViewportContextMenuAdvKitZoneExtender delegate */
	FDelegateHandle LevelViewportContextMenuAdvKitZoneExtenderDelegateHandle;
};

IMPLEMENT_MODULE(FAdvKitEditor, AdvKitEditor)

TSharedRef<FExtender> ExtendLevelViewportContextMenuForAdvKit(const TSharedRef<FUICommandList> CommandList, TArray<AActor*> SelectedActors);

FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors LevelViewportContextMenuAdvKitExtender;

void FAdvKitEditor::StartupModule()
{
	CommandList = MakeShareable(new FUICommandList);
	RegisterVisualizer<FAdvKitTraceComponentVisualizer, UAdvKitTraceUtilityComponent>();
	RegisterVisualizer<FAdvKitTransitionComponentPointVisualizer, UAdvKitTransitionComponentPoint>();
	RegisterVisualizer<FAdvKitTransitionComponentAreaVisualizer, UAdvKitTransitionComponentArea>();

	if (GIsEditor)
	{
		// Extend the level viewport context menu to handle zones
		LevelViewportContextMenuAdvKitExtender = FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateStatic(&ExtendLevelViewportContextMenuForAdvKit);
		FLevelEditorModule& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		auto& MenuExtenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();
		MenuExtenders.Add(LevelViewportContextMenuAdvKitExtender);
		LevelViewportContextMenuAdvKitZoneExtenderDelegateHandle = MenuExtenders.Last().GetHandle();
	}


}


void FAdvKitEditor::ShutdownModule()
{
	if (GUnrealEd != NULL)
	{
		GUnrealEd->UnregisterComponentVisualizer(UAdvKitTraceUtilityComponent::StaticClass()->GetFName());
		GUnrealEd->UnregisterComponentVisualizer(UAdvKitTransitionComponentPoint::StaticClass()->GetFName());
	}

	// Remove level viewport context menu extenders
	if (FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetAllLevelViewportContextMenuExtenders().RemoveAll([&](const FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors& Delegate) {
			return Delegate.GetHandle() == LevelViewportContextMenuAdvKitZoneExtenderDelegateHandle;
		});
	}
}


template<typename TVis, typename TComp> 
void FAdvKitEditor::RegisterVisualizer()
{
	if (GUnrealEd != NULL)
	{
		TSharedPtr<FComponentVisualizer> Visualizer = MakeShareable(new TVis());
		if (Visualizer.IsValid())
		{
			GUnrealEd->RegisterComponentVisualizer(TComp::StaticClass()->GetFName(), Visualizer);
			Visualizer->OnRegister();
		}
	}
}


#undef LOCTEXT_NAMESPACE