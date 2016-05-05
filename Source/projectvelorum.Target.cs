// Copyright 2012-2018 Helical Media Productions LLC. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class projectvelorumTarget : TargetRules
{
	public projectvelorumTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "projectvelorum" } );
	}
}
