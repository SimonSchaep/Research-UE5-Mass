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
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Avoidance);
	ExecutionOrder.ExecuteBefore.Add(UE::Mass::ProcessorGroupNames::Movement);
	bRequiresGameThreadExecution = true; //for navigationsystem
}

void UMovementProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	GameMode = Cast<ABattleSimGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMovementProcessor: GameMode not found."));
	}
}

void UMovementProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FAgentRadiusFragment>(EMassFragmentAccess::ReadOnly);
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

	//No multithreading cause navigation requires game thread
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
			const TConstArrayView<FAgentRadiusFragment> RadiusList = Context.GetFragmentView<FAgentRadiusFragment>();
			const TConstArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetFragmentView<FUnitTargetAcquisitionFragment>();
			const TArrayView<FMassVelocityFragment> VelocityList = Context.GetMutableFragmentView<FMassVelocityFragment>();
			const TArrayView<FMassForceFragment> ForceList = Context.GetMutableFragmentView<FMassForceFragment>();
			const TArrayView<FUnitAnimStateFragment> AnimStateList = Context.GetMutableFragmentView<FUnitAnimStateFragment>();
			const TArrayView<FMassMoveTargetFragment> NavTargetList = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
			const FMassMovementParameters& MovementParams = Context.GetConstSharedFragment<FMassMovementParameters>();
			const FUnitMoveParameters& UnitMoveParams = Context.GetConstSharedFragment<FUnitMoveParameters>();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				const FTransform& Transform = TransformList[EntityIndex].GetTransform();
				FMassMoveTargetFragment& MoveTarget = NavTargetList[EntityIndex];

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

				const FMassEntityHandle& TargetEntity = TargetAcquisitionList[EntityIndex].CurrentTarget;
				if (!EntityManager.IsEntityValid(TargetEntity)) continue;
				auto DataStruct = EntityManager.GetFragmentDataStruct(TargetEntity, FTransformFragment::StaticStruct());
				const FTransformFragment& TargetEntityTransform = DataStruct.Get<FTransformFragment>();

				//Set new target location
				UNavigationPath* NavigationPath = NavigationSystem->FindPathToLocationSynchronously(GetWorld(), Transform.GetLocation(), TargetEntityTransform.GetTransform().GetLocation());
				if (NavigationPath->PathPoints.Num() >= 2)
				{
					FVector Target = NavigationPath->PathPoints[1]; //Take point after starting point of path 
					MoveTarget.Center = Target;
				}
				else
				{
					MoveTarget.Center = TargetEntityTransform.GetTransform().GetLocation();
				}
				//DrawDebugLine(GetWorld(), TargetEntityTransform.GetTransform().GetLocation(), Transform.GetLocation(), FColor::Red);
				MoveTarget.DistanceToGoal = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				MoveTarget.Forward = (MoveTarget.Center - Transform.GetLocation()).GetSafeNormal();
				MoveTarget.DesiredSpeed = FMassInt16Real(MovementParams.DefaultDesiredSpeed);

				//Set anim state
				AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Moving;
			}
		}));
}
