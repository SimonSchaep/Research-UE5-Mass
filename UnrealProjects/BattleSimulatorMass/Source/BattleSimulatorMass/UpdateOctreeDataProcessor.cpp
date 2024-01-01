// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateOctreeDataProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "UnitFragments.h"
#include "TargetAcquisitionOctreeSubsystem.h"
#include "Spatial/SparseDynamicOctree3.h"

UUpdateOctreeDataProcessor::UUpdateOctreeDataProcessor()
	:EntityQuery{ *this }
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
}

void UUpdateOctreeDataProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionOctreeSubsystem>(Owner.GetWorld());
}

void UUpdateOctreeDataProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FUnitOctreeDataFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FAgentRadiusFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadOnly);
}

void UUpdateOctreeDataProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FUnitOctreeDataFragment> OctreeDataList = Context.GetMutableFragmentView<FUnitOctreeDataFragment>();
			const TConstArrayView<FArmyIdFragment> ArmyIdList = Context.GetFragmentView<FArmyIdFragment>();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				TargetAcquisitionSubsystem->UpdatePossibleTargetEntity(Context.GetEntity(EntityIndex), ArmyIdList[EntityIndex].ArmyId);
			}
		}));
}
