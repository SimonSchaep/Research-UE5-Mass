// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BattleSimulatorMass : ModuleRules
{
	public BattleSimulatorMass(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
        PrivateDefinitions.Add("ENABLE_MULTITHREADING");
        PrivateDefinitions.Add("ENABLE_SPATIAL");

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "StructUtils", "AIModule", "NavigationSystem", "GeometryCore", "AnimationSharing",
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
	}
}
