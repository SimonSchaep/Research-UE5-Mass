// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "UnitFragments.h"

UUnitProcessor::UUnitProcessor()
	: EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
}

void UUnitProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTargetAcquisitionFragment>(EMassFragmentAccess::ReadWrite, EMassFragmentPresence::Optional);
}

void UUnitProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([this](FMassExecutionContext& Context)
		{
			const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
			const TArrayView<FTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FTargetAcquisitionFragment>();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				//Acquire target

			}
		}));
}
