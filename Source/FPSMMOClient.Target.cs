// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class FPSMMOClientTarget : TargetRules
{
	public FPSMMOClientTarget( TargetInfo Target) : base(Target)
	{
        Type = TargetType.Client;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        ExtraModuleNames.Add("FPSMMO");
	}
}
