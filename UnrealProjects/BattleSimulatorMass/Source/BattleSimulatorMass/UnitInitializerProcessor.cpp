// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitInitializerProcessor.h"
#include "UnitFragments.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassDebugVisualizationComponent.h"
#include "MassGameplayDebugTypes.h"
#include "MassObserverRegistry.h"
#include "MassEQSSpawnDataGenerator.h"
#include "TargetAcquisitionSubsystem.h"
#include "TargetAcquisitionOctreeSubsystem.h"
#include "MassRepresentationFragments.h"
#include "MassRepresentationSubsystem.h"

//This class is based on UMassSpawnLocationProcessor, only some slight adjustments were made

UUnitInitializerProcessor::UUnitInitializerProcessor()
	:EntityQuery{ *this }
{
	bAutoRegisterWithProcessingPhases = false;
}

void UUnitInitializerProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

#ifdef ENABLE_SPATIAL
	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionOctreeSubsystem>(Owner.GetWorld());
#else
	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionSubsystem>(Owner.GetWorld());
#endif // ENABLE_SPATIAL	
}

void UUnitInitializerProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);	
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FAgentRadiusFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FUnitVisualizationParameters>(EMassFragmentPresence::All);
	EntityQuery.AddSharedRequirement<FMassRepresentationSubsystemSharedFragment>(EMassFragmentAccess::ReadOnly);
}

void UUnitInitializerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	if (!ensure(Context.ValidateAuxDataType<FMassEntitySpawnData>()))
	{
		UE_VLOG_UELOG(this, LogMass, Log, TEXT("Execution context has invalid AuxData or it's not FMassSpawnAuxData. Entity transforms won't be initialized."));
		return;
	}

	const UWorld* World = EntityManager.GetWorld();
	check(World);

	const ENetMode NetMode = World->GetNetMode();

	FMassEntitySpawnData& AuxData = Context.GetMutableAuxData().GetMutable<FMassEntitySpawnData>();
	TArray<FTransform>& Transforms = AuxData.Transforms;
	int UnitArmyId = AuxData.SpawnVars.ArmyId;
	FQuat SpawnRotation = AuxData.SpawnVars.SpawnRotationOverride;
	bool bShouldOverrideSpawnRotation = AuxData.SpawnVars.bShouldOverrideSpawnRotation;

	const int32 NumSpawnTransforms = Transforms.Num();
	if (NumSpawnTransforms == 0)
	{
		UE_VLOG_UELOG(this, LogMass, Error, TEXT("No spawn transforms provided. Entity transforms won't be initialized."));
		return;
	}

	int32 NumRequiredSpawnTransforms = 0;
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&NumRequiredSpawnTransforms](const FMassExecutionContext& Context)
		{
			NumRequiredSpawnTransforms += Context.GetNumEntities();
		});

	const int32 NumToAdd = NumRequiredSpawnTransforms - NumSpawnTransforms;
	if (NumToAdd > 0)
	{
		UE_VLOG_UELOG(this, LogMass, Warning,
			TEXT("Not enough spawn locations provided (%d) for all entities (%d). Existing locations will be reused randomly to fill the %d missing positions."),
			NumSpawnTransforms, NumRequiredSpawnTransforms, NumToAdd);

		Transforms.AddUninitialized(NumToAdd);
		for (int i = 0; i < NumToAdd; ++i)
		{
			Transforms[NumSpawnTransforms + i] = Transforms[FMath::RandRange(0, NumSpawnTransforms - 1)];
		}
	}

	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&](FMassExecutionContext& Context)
		{
			const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
			const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
			const TArrayView<FMassRepresentationFragment> RepresentationList = Context.GetMutableFragmentView<FMassRepresentationFragment>();
			const TConstArrayView<FAgentRadiusFragment> RadiusList = Context.GetFragmentView<FAgentRadiusFragment>();
			const FUnitVisualizationParameters& VisualizationParams = Context.GetConstSharedFragment<FUnitVisualizationParameters>();
			UMassRepresentationSubsystem* RepresentationSubsystem = Context.GetSharedFragment<FMassRepresentationSubsystemSharedFragment>().RepresentationSubsystem;

			if (VisualizationParams.LowResTemplateActors.IsEmpty())
			{
				UE_VLOG_UELOG(this, LogMass, Error, TEXT("No LowResTemplateActors configured. Entities won't be correctly initialized"));
				return;
			}
			if (VisualizationParams.HighResTemplateActors.IsEmpty())
			{
				UE_VLOG_UELOG(this, LogMass, Error, TEXT("No HighResTemplateActors configured. Entities won't be correctly initialized"));
				return;
			}
			if (VisualizationParams.StaticMeshInstanceDescs.IsEmpty())
			{
				UE_VLOG_UELOG(this, LogMass, Error, TEXT("No StaticMeshInstanceDescs configured. Entities won't be correctly initialized"));
				return;
			}

			TSubclassOf<AActor> LowResTemplateActor = VisualizationParams.LowResTemplateActors[FMath::Min(UnitArmyId, VisualizationParams.LowResTemplateActors.Num() - 1)];
			TSubclassOf<AActor> HighResTemplateActor = VisualizationParams.HighResTemplateActors[FMath::Min(UnitArmyId, VisualizationParams.HighResTemplateActors.Num() - 1)];
			FStaticMeshInstanceVisualizationDesc StaticMeshInstanceDesc = VisualizationParams.StaticMeshInstanceDescs[FMath::Min(UnitArmyId, VisualizationParams.StaticMeshInstanceDescs.Num() - 1)];

			const int32 NumEntities = Context.GetNumEntities();
			for (int32 EntityIndex{}; EntityIndex < NumEntities; ++EntityIndex)
			{
				//Spawn Transform
				const int32 AuxIndex = FMath::RandRange(0, Transforms.Num() - 1);
				TransformList[EntityIndex].GetMutableTransform() = Transforms[AuxIndex];
				if (bShouldOverrideSpawnRotation)
				{
					TransformList[EntityIndex].GetMutableTransform().SetRotation(SpawnRotation);
				}
				Transforms.RemoveAtSwap(AuxIndex, 1, /*bAllowShrinking=*/false);

				//Representation
				RepresentationList[EntityIndex].StaticMeshDescIndex = RepresentationSubsystem->FindOrAddStaticMeshDesc(StaticMeshInstanceDesc);
				RepresentationList[EntityIndex].LowResTemplateActorIndex = LowResTemplateActor.Get() ? RepresentationSubsystem->FindOrAddTemplateActor(LowResTemplateActor.Get()) : INDEX_NONE;
				RepresentationList[EntityIndex].HighResTemplateActorIndex = HighResTemplateActor.Get() ? RepresentationSubsystem->FindOrAddTemplateActor(HighResTemplateActor.Get()) : INDEX_NONE;

				//Army Id
				ArmyIdList[EntityIndex].ArmyId = UnitArmyId;

				//Add to subsystem
				TargetAcquisitionSubsystem->AddPossibleTargetEntity(
					Context.GetEntity(EntityIndex),
					UnitArmyId
				);
			}
		});
}
