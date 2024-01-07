// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleSimulator : ModuleRules
{
	public BattleSimulator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "NavigationSystem" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
