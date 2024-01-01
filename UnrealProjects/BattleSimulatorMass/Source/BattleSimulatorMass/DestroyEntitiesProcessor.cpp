// Fill out your copyright notice in the Description page of Project Settings.


#include "DestroyEntitiesProcessor.h"
#include "UnitTags.h"
#include "MassExecutionContext.h"

UDestroyEntitiesProcessor::UDestroyEntitiesProcessor()
	:EntityQuery{ *this }
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PostPhysics;
}

void UDestroyEntitiesProcessor::ConfigureQueries()
{
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::All);
}

void UDestroyEntitiesProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			EntityManager.Defer().DestroyEntities(Context.GetEntities());
		}));
}
