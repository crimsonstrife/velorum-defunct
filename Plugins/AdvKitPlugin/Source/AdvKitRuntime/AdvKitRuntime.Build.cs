// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

/** @brief Module to register runtime classes of the adventure kit. */
public class AdvKitRuntime : ModuleRules
{
    public AdvKitRuntime(TargetInfo Target)
    {
        PrivateIncludePaths.AddRange(new string[] { "AdvKitRuntime/Private" });
        PublicIncludePaths.AddRange(new string[] { "AdvKitRuntime/Public" });
 
        PublicDependencyModuleNames.AddRange(new string[] { 
			"Engine", 
			"Core", 
			"CoreUObject",
            "AIModule"
        });
    }
}