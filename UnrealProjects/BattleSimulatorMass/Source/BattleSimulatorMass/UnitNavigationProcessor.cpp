// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitNavigationProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassNavigationFragments.h"
#include "MassExecutionContext.h"
#include "MassMovementFragments.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "MassSimulationLOD.h"
#include "MassEntityView.h"
#include "Kismet/GameplayStatics.h"
#include "BattleSimGameMode.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"

UUnitNavigationProcessor::UUnitNavigationProcessor()
	:EntityQuery{ *this }
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
	bRequiresGameThreadExecution = true; //for navigationsystem
}

void UUnitNavigationProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	GameMode = Cast<ABattleSimGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UUnitNavigationProcessor: GameMode not found."));
	}
}

void UUnitNavigationProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FUnitTargetAcquisitionFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FMassMoveTargetFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddConstSharedRequirement<FUnitMoveParameters>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::None);
}

void UUnitNavigationProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	if (!GameMode->HasStartedSimulation()) return;

	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();
			const TConstArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetFragmentView<FUnitTargetAcquisitionFragment>();
			const TArrayView<FMassMoveTargetFragment> MoveTargetList = Context.GetMutableFragmentView<FMassMoveTargetFragment>();
			const FUnitMoveParameters& UnitMoveParams = Context.GetConstSharedFragment<FUnitMoveParameters>();

			//No parallel for cause NavigationSystem requires game thread
			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				if (TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr <= FMath::Square(UnitMoveParams.StopDistance))
				{
					continue;
				}

				//Get data
				FMassMoveTargetFragment& MoveTarget = MoveTargetList[EntityIndex];
				const FTransform& Transform = TransformList[EntityIndex].GetTransform();

				const FMassEntityHandle& TargetEntity = TargetAcquisitionList[EntityIndex].CurrentTarget;
				if (!EntityManager.IsEntityValid(TargetEntity)) continue;
				FMassEntityView EntityView{ EntityManager, TargetEntity };
				const FVector& TargetEntityLocation = EntityView.GetFragmentData<FTransformFragment>().GetTransform().GetLocation();

#ifdef ENABLE_MULTITHREADING
				FNavAgentProperties NavAgentProperties{};
				FPathFindingQuery NavParams{};
				NavParams.NavData = NavigationSystem->MainNavData;
				NavParams.QueryFilter = NavParams.NavData->GetDefaultQueryFilter();
				NavParams.StartLocation = Transform.GetLocation();
				NavParams.EndLocation = TargetEntityLocation;

				const FMassEntityHandle& Entity = Context.GetEntity(EntityIndex);

				FNavPathQueryDelegate Delegate;
				Delegate.BindLambda([&EntityManager, TargetEntityLocation, Entity, TargetEntity](uint32 PathId, ENavigationQueryResult::Type ResultType, FNavPathSharedPtr NavPath)
					{
						if (!EntityManager.IsEntityValid(TargetEntity)) return;
						if (!EntityManager.IsEntityValid(Entity)) return;

						FMassEntityView EntityView{ EntityManager, Entity };
						FMassMoveTargetFragment& MoveTarget = EntityView.GetFragmentData<FMassMoveTargetFragment>();

						if (NavPath->GetPathPoints().Num() >= 2 && ResultType == ENavigationQueryResult::Type::Success)
						{
							const FVector& Target = NavPath->GetPathPoints()[1]; //Take point after starting point of path 
							MoveTarget.Center = Target;
						}
						else
						{
							MoveTarget.Center = TargetEntityLocation;
						}
					});

				//Set temporary target until path is found
				MoveTarget.Center = TargetEntityLocation;

				//Find path
				NavigationSystem->FindPathAsync(NavAgentProperties, NavParams, Delegate, EPathFindingMode::Regular);
#else
				//Find path
				UNavigationPath* NavigationPath = NavigationSystem->FindPathToLocationSynchronously(GetWorld(), Transform.GetLocation(), TargetEntityLocation);
				if (NavigationPath->PathPoints.Num() >= 2)
				{
					const FVector& Target = NavigationPath->PathPoints[1]; //Take point after starting point of path 
					MoveTarget.Center = Target;
				}
				else
				{
					MoveTarget.Center = TargetEntityLocation;
				}
#endif // ENABLE_MULTITHREADING
			}
		}));
}