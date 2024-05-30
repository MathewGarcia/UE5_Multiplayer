// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FPSMMOServerTarget : TargetRules
{
	public FPSMMOServerTarget( TargetInfo Target) : base(Target)
	{
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        ExtraModuleNames.Add("FPSMMO");
	}
}
