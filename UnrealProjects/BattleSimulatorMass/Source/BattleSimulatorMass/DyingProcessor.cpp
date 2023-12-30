// Fill out your copyright notice in the Description page of Project Settings.


#include "DyingProcessor.h"
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
#include "TargetAcquisitionSubsystem.h"
#include "MassMovementFragments.h"

UDyingProcessor::UDyingProcessor()
	:EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PostPhysics;
}

void UDyingProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	SpawnerSubsystem = UWorld::GetSubsystem<UMassSpawnerSubsystem>(Owner.GetWorld());

	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionSubsystem>(Owner.GetWorld());
}

void UDyingProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FUnitDeathFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassRepresentationFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FUnitAnimStateFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassActorFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FUnitDeathParameters>(EMassFragmentPresence::All);
	EntityQuery.AddSharedRequirement<FMassRepresentationSubsystemSharedFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
}

void UDyingProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	//No multithreading, cause there would need to be too many mutexes
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FUnitDeathFragment> DeathList = Context.GetMutableFragmentView<FUnitDeathFragment>();
			const TArrayView<FMassRepresentationFragment> RepresentationList = Context.GetMutableFragmentView<FMassRepresentationFragment>();
			const TArrayView<FUnitAnimStateFragment> AnimStateList = Context.GetMutableFragmentView<FUnitAnimStateFragment>();
			const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
			const TConstArrayView<FMassActorFragment> ActorList = Context.GetFragmentView<FMassActorFragment>();
			const TConstArrayView<FArmyIdFragment> ArmyIdList = Context.GetFragmentView<FArmyIdFragment>();
			const FUnitDeathParameters& DeathParams = Context.GetConstSharedFragment<FUnitDeathParameters>();
			UMassRepresentationSubsystem* RepresentationSubsystem = Context.GetSharedFragment<FMassRepresentationSubsystemSharedFragment>().RepresentationSubsystem;
			const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				//first time logic
				if (DeathList[EntityIndex].DeathDelayTimer + FLT_EPSILON >= DeathParams.AnimationDeathDelay)
				{
					const FMassEntityHandle& Entity = Context.GetEntity(EntityIndex);

					TargetAcquisitionSubsystem->RemovePossibleTargetEntity(Entity);

					Context.Defer().RemoveFragment<FMassForceFragment>(Entity);
					Context.Defer().RemoveFragment<FMassVelocityFragment>(Entity);

					AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Dead;
				}
				//Timer
				if (DeathList[EntityIndex].DeathDelayTimer > 0)
				{
					DeathList[EntityIndex].DeathDelayTimer -= WorldDeltaTime;

					if (DeathList[EntityIndex].DeathDelayTimer > 0) continue;
				}

				//Mark for destroy
				Context.Defer().AddTag<FDeadTag>(Context.GetEntity(EntityIndex));

				FTransform Transform = TransformList[EntityIndex].GetTransform();
				int ArmyId = ArmyIdList[EntityIndex].ArmyId;

				//Create new dead entity
				Context.Defer().PushCommand<FMassDeferredCreateCommand>(
					[this, RepresentationSubsystem, DeathParams, Transform, ArmyId](FMassEntityManager& EntityManager)
					{
						//Spawn entity				
						TArray<FMassEntityHandle> SpawnedEntities{};
						const FMassEntityTemplate EntityTemplate = DeathParams.DeadEntityConfig->GetConfig().GetOrCreateEntityTemplate(*GetWorld());
						SpawnerSubsystem->SpawnEntities(EntityTemplate, 1, SpawnedEntities);

						//Set transform
						auto DataStruct = EntityManager.GetFragmentDataStruct(SpawnedEntities[0], FTransformFragment::StaticStruct());
						FTransformFragment& SpawnedEntityTransform = DataStruct.Get<FTransformFragment>();
						SpawnedEntityTransform.SetTransform(Transform);

						//Set representation
						auto view = FMassEntityView(EntityManager, SpawnedEntities[0]);
						const FUnitVisualizationParameters& SpawnedEntityVisualizationParams = view.GetConstSharedFragmentData<FUnitVisualizationParameters>();
						DataStruct = EntityManager.GetFragmentDataStruct(SpawnedEntities[0], FMassRepresentationFragment::StaticStruct());
						FMassRepresentationFragment& SpawnedEntityRepresentation = DataStruct.Get<FMassRepresentationFragment>();

						TSubclassOf<AActor> LowResTemplateActor = SpawnedEntityVisualizationParams.LowResTemplateActors[FMath::Min(ArmyId, SpawnedEntityVisualizationParams.LowResTemplateActors.Num() - 1)];
						TSubclassOf<AActor> HighResTemplateActor = SpawnedEntityVisualizationParams.HighResTemplateActors[FMath::Min(ArmyId, SpawnedEntityVisualizationParams.HighResTemplateActors.Num() - 1)];
						FStaticMeshInstanceVisualizationDesc StaticMeshInstanceDesc = SpawnedEntityVisualizationParams.StaticMeshInstanceDescs[FMath::Min(ArmyId, SpawnedEntityVisualizationParams.StaticMeshInstanceDescs.Num() - 1)];

						SpawnedEntityRepresentation.StaticMeshDescIndex = RepresentationSubsystem->FindOrAddStaticMeshDesc(StaticMeshInstanceDesc);
						SpawnedEntityRepresentation.LowResTemplateActorIndex = LowResTemplateActor.Get() ? RepresentationSubsystem->FindOrAddTemplateActor(LowResTemplateActor.Get()) : INDEX_NONE;
						SpawnedEntityRepresentation.HighResTemplateActorIndex = HighResTemplateActor.Get() ? RepresentationSubsystem->FindOrAddTemplateActor(HighResTemplateActor.Get()) : INDEX_NONE;
					});
			}
		}));
}