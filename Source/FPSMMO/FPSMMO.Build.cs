// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FPSMMO : ModuleRules
{
	public FPSMMO(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG" , "OnlineSubsystem", "OnlineSubsystemUtils", "Networking","Sockets","PhysicsCore","Landscape"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" , "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemNull","OnlineSubsystemSteam", "AdvancedSessions", "Niagara", "ProceduralMeshComponent", "DynamicMesh", "GeometryFramework" });


		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });


		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
