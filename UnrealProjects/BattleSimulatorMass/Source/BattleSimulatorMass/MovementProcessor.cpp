// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassNavigationFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "MassSimulationLOD.h"
#include "Kismet/GameplayStatics.h"
#include "BattleSimGameMode.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"

UMovementProcessor::UMovementProcessor()
	:EntityQuery{ *this }
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	//Defined order, so we can stop all avoidance velocity and forces when we don't want to move
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
}

void UMovementProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	GameMode = Cast<ABattleSimGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMovementProcessor: GameMode not found."));
	}
}

void UMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FUnitTargetAcquisitionFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FUnitAnimStateFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassVelocityFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FMassForceFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FMassMovementParameters>(EMassFragmentPresence::All);
	EntityQuery.AddConstSharedRequirement<FUnitMoveParameters>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::None);
}

void UMovementProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	if (!GameMode->HasStartedSimulation()) return;

	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
			const TConstArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetFragmentView<FUnitTargetAcquisitionFragment>();
			const TArrayView<FMassVelocityFragment> VelocityList = Context.GetMutableFragmentView<FMassVelocityFragment>();
			const TArrayView<FMassForceFragment> ForceList = Context.GetMutableFragmentView<FMassForceFragment>();
			const TArrayView<FUnitAnimStateFragment> AnimStateList = Context.GetMutableFragmentView<FUnitAnimStateFragment>();
			const TArrayView<FMassMoveTargetFragment> MoveTargetList = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
			const FMassMovementParameters& MovementParams = Context.GetConstSharedFragment<FMassMovementParameters>();
			const FUnitMoveParameters& UnitMoveParams = Context.GetConstSharedFragment<FUnitMoveParameters>();

#ifdef ENABLE_MULTITHREADING
			ParallelFor(Context.GetNumEntities(), [&](int32 EntityIndex)
				{
					const FTransform& Transform = TransformList[EntityIndex].GetTransform();
					FMassMoveTargetFragment& MoveTarget = MoveTargetList[EntityIndex];

					//If in stop range -> stop moving
					if (TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr <= FMath::Square(UnitMoveParams.StopDistance))
					{
						MoveTarget.Center = Transform.GetLocation();
						MoveTarget.DistanceToGoal = 0;
						VelocityList[EntityIndex].Value = FVector::Zero();
						ForceList[EntityIndex].Value = FVector::Zero();

						//Set anim state
						if (AnimStateList[EntityIndex].UnitAnimState == EUnitAnimState::Moving)
						{
							AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Idle;
						}
						return;
					}

					//Set MoveTarget values
					MoveTarget.DistanceToGoal = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
					MoveTarget.Forward = (MoveTarget.Center - Transform.GetLocation()).GetSafeNormal();
					MoveTarget.DesiredSpeed = FMassInt16Real(MovementParams.DefaultDesiredSpeed);

					//Set anim state
					AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Moving;
				});
#else
			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				const FTransform& Transform = TransformList[EntityIndex].GetTransform();
				FMassMoveTargetFragment& MoveTarget = MoveTargetList[EntityIndex];

				//If in stop range -> stop moving
				if (TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr <= FMath::Square(UnitMoveParams.StopDistance))
				{
					MoveTarget.Center = Transform.GetLocation();
					MoveTarget.DistanceToGoal = 0;
					VelocityList[EntityIndex].Value = FVector::Zero();
					ForceList[EntityIndex].Value = FVector::Zero();

					//Set anim state
					if (AnimStateList[EntityIndex].UnitAnimState == EUnitAnimState::Moving)
					{
						AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Idle;
					}
					continue;
				}

				//Set MoveTarget values
				MoveTarget.DistanceToGoal = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				MoveTarget.Forward = (MoveTarget.Center - Transform.GetLocation()).GetSafeNormal();
				MoveTarget.DesiredSpeed = FMassInt16Real(MovementParams.DefaultDesiredSpeed);

				//Set anim state
				AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Moving;
			}
#endif // ENABLE_MULTITHREADING
		}));
}
