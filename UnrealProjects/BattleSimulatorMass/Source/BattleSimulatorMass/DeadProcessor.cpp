// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadProcessor.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "MassExecutionContext.h"
#include "MassRepresentationFragments.h"
#include "MassRepresentationSubsystem.h"
#include "MassActorSubsystem.h"
#include "MassCommonTypes.h"
#include "MassSpawnerTypes.h"
#include "MassEntityConfigAsset.h"
#include "MassSpawnerSubsystem.h"
#include "MassEntityView.h"

UDeadProcessor::UDeadProcessor()
	:EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Representation);
}

void UDeadProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	SpawnerSubsystem = UWorld::GetSubsystem<UMassSpawnerSubsystem>(Owner.GetWorld());
}

void UDeadProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FUnitDeathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FUnitDeathParameters>(EMassFragmentPresence::All);
	EntityQuery.AddSharedRequirement<FMassRepresentationSubsystemSharedFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
}

void UDeadProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FUnitDeathFragment> DeathList = Context.GetMutableFragmentView<FUnitDeathFragment>();
			const TArrayView<FMassRepresentationFragment> RepresentationList = Context.GetMutableFragmentView<FMassRepresentationFragment>();
			const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
			const TConstArrayView<FMassActorFragment> ActorList = Context.GetFragmentView<FMassActorFragment>();
			const TConstArrayView<FArmyIdFragment> ArmyIdList = Context.GetFragmentView<FArmyIdFragment>();
			const FUnitDeathParameters& DeathParams = Context.GetConstSharedFragment<FUnitDeathParameters>();
			UMassRepresentationSubsystem* RepresentationSubsystem = Context.GetSharedFragment<FMassRepresentationSubsystemSharedFragment>().RepresentationSubsystem;
			const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				//Timer
				if (DeathList[EntityIndex].DeathDelayTimer > 0)
				{
					DeathList[EntityIndex].DeathDelayTimer -= WorldDeltaTime;

					if (DeathList[EntityIndex].DeathDelayTimer > 0) continue;
				}

				//Spawn dead entity				
				TArray<FMassEntityHandle> SpawnedEntities{};
				const FMassEntityTemplate EntityTemplate = DeathParams.DeadEntityConfig->GetConfig().GetOrCreateEntityTemplate(*GetWorld());
				SpawnerSubsystem->SpawnEntities(EntityTemplate, 1, SpawnedEntities);

				//Set transform
				auto DataStruct = EntityManager.GetFragmentDataStruct(SpawnedEntities[0], FTransformFragment::StaticStruct());
				FTransformFragment& SpawnedEntityTransform = DataStruct.GetMutable<FTransformFragment>();
				SpawnedEntityTransform.SetTransform(TransformList[EntityIndex].GetTransform());

				//Set representation
				auto view = FMassEntityView(EntityManager, SpawnedEntities[0]);
				const FUnitVisualizationParameters& SpawnedEntityVisualizationParams = view.GetConstSharedFragmentData<FUnitVisualizationParameters>();
				DataStruct = EntityManager.GetFragmentDataStruct(SpawnedEntities[0], FMassRepresentationFragment::StaticStruct());
				FMassRepresentationFragment& SpawnedEntityRepresentation = DataStruct.GetMutable<FMassRepresentationFragment>();

				TSubclassOf<AActor> LowResTemplateActor = SpawnedEntityVisualizationParams.LowResTemplateActors[FMath::Min(ArmyIdList[EntityIndex].ArmyId, SpawnedEntityVisualizationParams.LowResTemplateActors.Num() - 1)];
				TSubclassOf<AActor> HighResTemplateActor = SpawnedEntityVisualizationParams.HighResTemplateActors[FMath::Min(ArmyIdList[EntityIndex].ArmyId, SpawnedEntityVisualizationParams.HighResTemplateActors.Num() - 1)];
				FStaticMeshInstanceVisualizationDesc StaticMeshInstanceDesc = SpawnedEntityVisualizationParams.StaticMeshInstanceDescs[FMath::Min(ArmyIdList[EntityIndex].ArmyId, SpawnedEntityVisualizationParams.StaticMeshInstanceDescs.Num() - 1)];

				SpawnedEntityRepresentation.StaticMeshDescIndex = RepresentationSubsystem->FindOrAddStaticMeshDesc(StaticMeshInstanceDesc);
				SpawnedEntityRepresentation.LowResTemplateActorIndex = LowResTemplateActor.Get() ? RepresentationSubsystem->FindOrAddTemplateActor(LowResTemplateActor.Get()) : INDEX_NONE;
				SpawnedEntityRepresentation.HighResTemplateActorIndex = HighResTemplateActor.Get() ? RepresentationSubsystem->FindOrAddTemplateActor(HighResTemplateActor.Get()) : INDEX_NONE;

				//Kill this entity
				Context.Defer().DestroyEntity(Context.GetEntity(EntityIndex));
			}
		}));
}
