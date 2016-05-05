// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

/** @brief Module to register editor classes of the adventure kit. */
public class AdvKitEditor : ModuleRules
{
    public AdvKitEditor(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(new string[] { 
            "AdvKitEditor/Private",
        });

        PublicIncludePaths.AddRange(new string[] { 
			"Core",
            "Developer/AssetTools/Public",
            "ActorPickerMode",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "AdvKitRuntime",
            "EditorStyle"
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "InputCore",
            "Engine", 
            "Core", 
            "CoreUObject", 
			"Slate",
			"SlateCore",
            "UnrealEd", 
            "AssetTools",
            "ActorPickerMode", 
            "AdvKitRuntime" 
        });
    }
}