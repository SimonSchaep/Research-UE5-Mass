// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitTraits.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"
#include "Translators/MassSceneComponentLocationTranslator.h"

// ArmyId
void UArmyIdTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FArmyIdFragment>();
}

// Attack
void UAttackTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct AttackParamsFragment = EntityManager.GetOrCreateConstSharedFragment(AttackParameters);
	BuildContext.AddConstSharedFragment(AttackParamsFragment);

	BuildContext.AddFragment<FUnitAttackFragment>();
}

// DeathHandling
void UDeathHandlingTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct DeathParamsFragment = EntityManager.GetOrCreateConstSharedFragment(DeathParameters);
	BuildContext.AddConstSharedFragment(DeathParamsFragment);

	FUnitDeathFragment& DeathTemplate = BuildContext.AddFragment_GetRef<FUnitDeathFragment>();
	DeathTemplate.DeathDelayTimer = DeathParameters.AnimationDeathDelay;
}

// Health
void UHealthTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FUnitHealthFragment& HealthTemplate = BuildContext.AddFragment_GetRef<FUnitHealthFragment>();
	HealthTemplate.CurrentHealth = Health;
}

// Move
void UMoveTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct MoveParamsFragment = EntityManager.GetOrCreateConstSharedFragment(MoveParameters);
	BuildContext.AddConstSharedFragment(MoveParamsFragment);
}

// TargetAcquisition
void UTargetAcquisitionTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment_GetRef<FUnitTargetAcquisitionFragment>();
}

// Animation
void UUnitAnimationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct AnimParamsFragment = EntityManager.GetOrCreateConstSharedFragment(AnimationParameters);
	BuildContext.AddConstSharedFragment(AnimParamsFragment);

	BuildContext.AddFragment<FUnitAnimStateFragment>();
}

// OrientationSync
void UUnitOrientationSyncTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FTransformFragment>();
	BuildContext.RequireFragment<FMassSceneComponentWrapperFragment>();

	BuildContext.AddTag<FSyncRotationTag>();
}

// Visualization
void UUnitVisualizationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct VisualizationParamsFragment = EntityManager.GetOrCreateConstSharedFragment(VisualizationParameters);
	BuildContext.AddConstSharedFragment(VisualizationParamsFragment);
}
