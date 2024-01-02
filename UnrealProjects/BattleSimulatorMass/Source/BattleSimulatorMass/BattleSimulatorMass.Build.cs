// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleSimulatorMass : ModuleRules
{
	public BattleSimulatorMass(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        //PrivateDefinitions.Add("ENABLE_MULTITHREADING");
        //PrivateDefinitions.Add("ENABLE_SPATIAL");

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "StructUtils", "AIModule", "NavigationSystem", "GeometryCore",
            "MassAIDebug", 
			"MassEntity", 
			"MassActors", 
			"MassSignals",
            "MassSimulation",
            "MassRepresentation", 
			"MassReplication", 
			"MassSpawner",
            "MassCommon", 
			"MassCrowd",
            "MassLOD", 
			"MassNavigation", 
			"MassMovement", 
			"MassGameplayDebug" });

        PrecompileForTargets = PrecompileTargetsType.Any;

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
