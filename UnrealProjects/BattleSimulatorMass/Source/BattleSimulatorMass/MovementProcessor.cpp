// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassNavigationFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "UnitFragments.h"
#include "MassSimulationLOD.h"

UMovementProcessor::UMovementProcessor()
	:EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	//ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::Movement;
	//ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	//ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
	//ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Behavior);
}

void UMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FAgentRadiusFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FUnitTargetAcquisitionFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FUnitAnimStateFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
	EntityQuery.AddConstSharedRequirement<FUnitMoveParameters>(EMassFragmentPresence::All);
}

void UMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
			const TArrayView<FAgentRadiusFragment> RadiusList = Context.GetMutableFragmentView<FAgentRadiusFragment>();
			const TArrayView<FMassVelocityFragment> VelocityList = Context.GetMutableFragmentView<FMassVelocityFragment>();
			const TArrayView<FUnitAnimStateFragment> AnimStateList = Context.GetMutableFragmentView<FUnitAnimStateFragment>();
			const TArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FUnitTargetAcquisitionFragment>();
			const TArrayView<FMassMoveTargetFragment> NavTargetList = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
			const FMassMovementParameters& MovementParams = Context.GetConstSharedFragment<FMassMovementParameters>();
			const FUnitMoveParameters& UnitMoveParams = Context.GetConstSharedFragment<FUnitMoveParameters>();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				const FTransform& Transform = TransformList[EntityIndex].GetTransform();
				FMassMoveTargetFragment& MoveTarget = NavTargetList[EntityIndex];
				if (TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr <= FMath::Square(UnitMoveParams.StopDistance))
				{
					MoveTarget.Center = Transform.GetLocation();
					MoveTarget.DistanceToGoal = 0;
					VelocityList[EntityIndex].Value = FVector::Zero();

					//Set anim state
					AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Attacking;
					continue;
				}

				FMassEntityHandle& TargetEntity = TargetAcquisitionList[EntityIndex].CurrentTarget;				
				if (!EntityManager.IsEntityValid(TargetEntity)) continue;
				auto DataStruct = EntityManager.GetFragmentDataStruct(TargetEntity, FTransformFragment::StaticStruct());
				if (!DataStruct.IsValid()) continue;
				const FTransformFragment& TargetEntityTransform = DataStruct.Get<FTransformFragment>();

				//Set new target location
				MoveTarget.Center = TargetEntityTransform.GetTransform().GetLocation();
				MoveTarget.DistanceToGoal = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				MoveTarget.Forward = (MoveTarget.Center - Transform.GetLocation()).GetSafeNormal();
				MoveTarget.DesiredSpeed = FMassInt16Real(MovementParams.DefaultDesiredSpeed);

				//Set anim state
				AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Moving;
			}
		}));
}
