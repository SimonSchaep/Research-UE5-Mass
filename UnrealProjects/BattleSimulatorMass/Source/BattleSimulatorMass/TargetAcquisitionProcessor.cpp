// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionProcessor.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "UnitFragments.h"
#include "TargetAcquisitionSubsystem.h"

UTargetAcquisitionProcessor::UTargetAcquisitionProcessor()
	: EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
}

void UTargetAcquisitionProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionSubsystem>(Owner.GetWorld());
}

void UTargetAcquisitionProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTargetAcquisitionFragment>(EMassFragmentAccess::ReadWrite);
}

void UTargetAcquisitionProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
			const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();
			const TArrayView<FTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FTargetAcquisitionFragment>();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				//Acquire target
				float& ClosestDistanceSqr = TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr;
				ClosestDistanceSqr = FLT_MAX;

				auto EntitiesArrays = TargetAcquisitionSubsystem->GetPossibleTargetEntities();
				for (int32 ArrayIndex{}; ArrayIndex < EntitiesArrays.Num(); ++ArrayIndex)
				{
					if (ArrayIndex == ArmyIdList[EntityIndex].ArmyId)
					{
						continue;
					}

					for (FMassEntityHandle& Handle : EntitiesArrays[ArrayIndex])
					{
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
}
