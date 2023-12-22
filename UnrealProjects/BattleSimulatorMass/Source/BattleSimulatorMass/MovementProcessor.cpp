// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassNavigationFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "UnitFragments.h"

UMovementProcessor::UMovementProcessor()
	:EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
}

void UMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTargetAcquisitionFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
}

void UMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
			const TArrayView<FTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FTargetAcquisitionFragment>();
			const TArrayView<FMassMoveTargetFragment> NavTargetList = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
			const FMassMovementParameters& MovementParams = Context.GetConstSharedFragment<FMassMovementParameters>();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				const FTransform& Transform = TransformList[EntityIndex].GetTransform();
				FMassEntityHandle& TargetEntity = TargetAcquisitionList[EntityIndex].CurrentTarget;				
				if (!EntityManager.IsEntityValid(TargetEntity)) continue;
				auto DataStruct = EntityManager.GetFragmentDataStruct(TargetEntity, FTransformFragment::StaticStruct());
				if (!DataStruct.IsValid()) continue;
				const FTransformFragment& TargetEntityTransform = DataStruct.Get<FTransformFragment>();
				FMassMoveTargetFragment& MoveTarget = NavTargetList[EntityIndex];

				//Set new target location				
				MoveTarget.Center = TargetEntityTransform.GetTransform().GetLocation();
				MoveTarget.DistanceToGoal = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				MoveTarget.Forward = (MoveTarget.Center - Transform.GetLocation()).GetSafeNormal();
				MoveTarget.DesiredSpeed = FMassInt16Real(MovementParams.DefaultDesiredSpeed);
			}
		}));
}
