// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "TargetAcquisitionSubsystem.h"
#include "TargetAcquisitionOctreeSubsystem.h"
#include "MassEntityView.h"
#include "Kismet/GameplayStatics.h"
#include "BattleSimGameMode.h"
#include "Async/ParallelFor.h"

UTargetAcquisitionProcessor::UTargetAcquisitionProcessor()
	: EntityQuery{ *this }
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
	ProcessingPhase = EMassProcessingPhase::PrePhysics;
}

void UTargetAcquisitionProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

#ifdef ENABLE_SPATIAL
	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionOctreeSubsystem>(Owner.GetWorld());
#else
	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionSubsystem>(Owner.GetWorld());
#endif // ENABLE_SPATIAL	

	GameMode = Cast<ABattleSimGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UMovementProcessor: GameMode not found."));
	}
}

void UTargetAcquisitionProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FUnitTargetAcquisitionFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::None);
}

void UTargetAcquisitionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	if (!GameMode->HasStartedSimulation()) return;
	
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
			const TArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FUnitTargetAcquisitionFragment>();
			const TConstArrayView<FTransformFragment> TransformList = Context.GetFragmentView<FTransformFragment>();

#ifdef ENABLE_MULTITHREADING
			ParallelFor(Context.GetNumEntities(), [&](int32 EntityIndex)
#else
			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
#endif // ENABLE_MULTITHREADING
			{
				//Acquire target
				float& ClosestDistanceSqr = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				ClosestDistanceSqr = FLT_MAX;

#ifdef ENABLE_SPATIAL
				auto& Octrees = TargetAcquisitionSubsystem->GetOctrees();

				for (int32 OctreeIndex{}; OctreeIndex < Octrees.Num(); ++OctreeIndex)
				{
					//Skip those with the same army id
					if (OctreeIndex == ArmyIdList[EntityIndex].ArmyId)
					{
						continue;
					}

					//Approximate the closest target
					Octrees[OctreeIndex].FindNearbyElements(TransformList[EntityIndex].GetTransform().GetLocation(), [&](const FUnitOctreeElement& OctreeElement)
						{
							const FMassEntityHandle& Handle = OctreeElement.EntityHandle;

							if (!EntityManager.IsEntityValid(Handle)) return;

							auto HandleTransform = EntityManager.GetFragmentDataStruct(Handle, FTransformFragment::StaticStruct()).Get<FTransformFragment>();
							float DistanceSqr = FVector::DistSquared(TransformList[EntityIndex].GetTransform().GetLocation(), HandleTransform.GetTransform().GetLocation());
							if (DistanceSqr < ClosestDistanceSqr)
							{
								TargetAcquisitionList[EntityIndex].CurrentTarget = Handle;
								ClosestDistanceSqr = DistanceSqr;
							}
						});


					//Do a precise bounds check to see if there's any closer targets
					float ClosestDistance = FMath::Sqrt(ClosestDistanceSqr);
					FBoxCenterAndExtent Bounds{
						TransformList[EntityIndex].GetTransform().GetLocation(),
						FVector{ClosestDistance,ClosestDistance,ClosestDistance}
					};

					Octrees[OctreeIndex].FindElementsWithBoundsTest(Bounds, [&](const FUnitOctreeElement& OctreeElement)
						{
							const FMassEntityHandle& Handle = OctreeElement.EntityHandle;

							if (!EntityManager.IsEntityValid(Handle)) return;

							auto HandleTransform = EntityManager.GetFragmentDataStruct(Handle, FTransformFragment::StaticStruct()).Get<FTransformFragment>();
							float DistanceSqr = FVector::DistSquared(TransformList[EntityIndex].GetTransform().GetLocation(), HandleTransform.GetTransform().GetLocation());
							if (DistanceSqr < ClosestDistanceSqr)
							{
								TargetAcquisitionList[EntityIndex].CurrentTarget = Handle;
								ClosestDistanceSqr = DistanceSqr;
							}
						});
				}
#else
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
#endif // ENABLE_SPATIAL

				//No target was found
				if (ClosestDistanceSqr == FLT_MAX)
				{
					ClosestDistanceSqr = 0;
					TargetAcquisitionList[EntityIndex].CurrentTarget = EntityManager.InvalidEntity;
				}
			}			
#ifdef ENABLE_MULTITHREADING
			);
#endif // ENABLE_MULTITHREADING
		}));
}
