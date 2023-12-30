// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "TargetAcquisitionSubsystem.h"
#include "MassEntityView.h"
#include "Kismet/GameplayStatics.h"
#include "BattleSimGameMode.h"
#include "Async/ParallelFor.h"

UTargetAcquisitionProcessor::UTargetAcquisitionProcessor()
	: EntityQuery(*this)
{
	//Will be registered on input in gamemode
	bAutoRegisterWithProcessingPhases = true;
	ProcessingPhase = EMassProcessingPhase::FrameEnd;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);

	
}

void UTargetAcquisitionProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionSubsystem>(Owner.GetWorld());

	GameMode = Cast<ABattleSimGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMovementProcessor: GameMode not found."));
	}
}

void UTargetAcquisitionProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FUnitTargetAcquisitionFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::None);
}

void UTargetAcquisitionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
#ifdef ENABLE_MULTITHREADING
	if (!GameMode->HasStartedSimulation()) return;
	
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
			const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
			const TArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FUnitTargetAcquisitionFragment>();

			ParallelFor(Context.GetNumEntities(), [&](int32 EntityIndex)
			{
				//Acquire target
				float& ClosestDistanceSqr = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				ClosestDistanceSqr = FLT_MAX;

				//Loop over all entities in TargetAcquisitionSubsystem
				auto& EntitiesArrays = TargetAcquisitionSubsystem->GetPossibleTargetEntities();
				for (int32 ArrayIndex{}; ArrayIndex < EntitiesArrays.Num(); ++ArrayIndex)
				{
					//Skip those with the same army id
					if (ArrayIndex == ArmyIdList[EntityIndex].ArmyId)
					{
						continue;
					}

					//Find closest entity
					for (const FMassEntityHandle& Handle : EntitiesArrays[ArrayIndex])
					{
						if (!EntityManager.IsEntityValid(Handle)) continue;

						auto HandleTransform = EntityManager.GetFragmentDataStruct(Handle, FTransformFragment::StaticStruct()).Get<FTransformFragment>();
						float DistanceSqr = FVector::DistSquared(TransformList[EntityIndex].GetTransform().GetLocation(), HandleTransform.GetTransform().GetLocation());
						if (DistanceSqr < ClosestDistanceSqr)
						{
							TargetAcquisitionList[EntityIndex].CurrentTarget = Handle;
							ClosestDistanceSqr = DistanceSqr;
						}
					}
				}
			});
		}));
#else
	if (!GameMode->HasStartedSimulation()) return;

	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
			const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
			const TArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FUnitTargetAcquisitionFragment>();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				//Acquire target
				float& ClosestDistanceSqr = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				ClosestDistanceSqr = FLT_MAX;

				//Loop over all entities in TargetAcquisitionSubsystem
				auto& EntitiesArrays = TargetAcquisitionSubsystem->GetPossibleTargetEntities();
				for (int32 ArrayIndex{}; ArrayIndex < EntitiesArrays.Num(); ++ArrayIndex)
				{
					//Skip those with the same army id
					if (ArrayIndex == ArmyIdList[EntityIndex].ArmyId)
					{
						continue;
					}

					//Find closest entity
					for (const FMassEntityHandle& Handle : EntitiesArrays[ArrayIndex])
					{
						if (!EntityManager.IsEntityValid(Handle)) continue;

						auto HandleTransform = EntityManager.GetFragmentDataStruct(Handle, FTransformFragment::StaticStruct()).Get<FTransformFragment>();
						float DistanceSqr = FVector::DistSquared(TransformList[EntityIndex].GetTransform().GetLocation(), HandleTransform.GetTransform().GetLocation());
						if (DistanceSqr < ClosestDistanceSqr)
						{
							TargetAcquisitionList[EntityIndex].CurrentTarget = Handle;
							ClosestDistanceSqr = DistanceSqr;
						}
					}
				}
			}
		}));
#endif // ENABLE_MULTITHREADING	
}
