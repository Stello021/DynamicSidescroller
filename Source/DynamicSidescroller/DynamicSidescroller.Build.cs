// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class DynamicSidescroller : ModuleRules
{
	public DynamicSidescroller(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "PhysicsCore" });
	}
}
