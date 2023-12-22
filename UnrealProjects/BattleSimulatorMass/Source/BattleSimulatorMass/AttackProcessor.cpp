// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackProcessor.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "MassExecutionContext.h"
#include "TargetAcquisitionSubsystem.h"

UAttackProcessor::UAttackProcessor()
	:EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = int32(EProcessorExecutionFlags::All);
}

void UAttackProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionSubsystem>(Owner.GetWorld());
}

void UAttackProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FAttackFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FTargetAcquisitionFragment>(EMassFragmentAccess::ReadOnly);
}

void UAttackProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FAttackFragment> AttackList = Context.GetMutableFragmentView<FAttackFragment>();
			const TArrayView<FTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FTargetAcquisitionFragment>();
			const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				if (AttackList[EntityIndex].AttackDelayTimer > 0)
				{
					AttackList[EntityIndex].AttackDelayTimer -= WorldDeltaTime;
				}

				if (TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr <= AttackList[EntityIndex].RangeSqr && AttackList[EntityIndex].AttackDelayTimer <= 0)
				{
					AttackList[EntityIndex].AttackDelayTimer += AttackList[EntityIndex].AttackDelay;

					FMassEntityHandle& TargetEntity = TargetAcquisitionList[EntityIndex].CurrentTarget;
					if (!TargetEntity.IsValid()) continue;
					auto DataStruct = EntityManager.GetFragmentDataStruct(TargetEntity, FHealthFragment::StaticStruct());
					if (!DataStruct.IsValid()) continue;
					FHealthFragment& TargetEntityHealth = DataStruct.Get<FHealthFragment>();
					TargetEntityHealth.CurrentHealth -= AttackList[EntityIndex].Damage;
					if (TargetEntityHealth.CurrentHealth <= 0)
					{
						TargetAcquisitionSubsystem->RemovePossibleTargetEntity(TargetEntity);
						Context.Defer().DestroyEntity(TargetEntity);
					}
				}				
			}
		}));
}
