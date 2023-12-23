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
	EntityQuery.AddRequirement<FUnitAttackFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FUnitTargetAcquisitionFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FUnitAttackParameters>(EMassFragmentPresence::All);
}

void UAttackProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FUnitAttackFragment> AttackList = Context.GetMutableFragmentView<FUnitAttackFragment>();
			const TArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetMutableFragmentView<FUnitTargetAcquisitionFragment>();
			const FUnitAttackParameters& AttackParams = Context.GetConstSharedFragment<FUnitAttackParameters>();
			const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				if (AttackList[EntityIndex].AttackDelayTimer > 0)
				{
					AttackList[EntityIndex].AttackDelayTimer -= WorldDeltaTime;
				}

				if (TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr <= FMath::Square(AttackParams.Range) && AttackList[EntityIndex].AttackDelayTimer <= 0)
				{
					AttackList[EntityIndex].AttackDelayTimer += AttackParams.AttackDelay;

					FMassEntityHandle& TargetEntity = TargetAcquisitionList[EntityIndex].CurrentTarget;
					if (!TargetEntity.IsValid()) continue;
					auto DataStruct = EntityManager.GetFragmentDataStruct(TargetEntity, FUnitHealthFragment::StaticStruct());
					if (!DataStruct.IsValid()) continue;
					FUnitHealthFragment& TargetEntityHealth = DataStruct.Get<FUnitHealthFragment>();
					TargetEntityHealth.CurrentHealth -= AttackParams.Damage;
					if (TargetEntityHealth.CurrentHealth <= 0)
					{
						TargetAcquisitionSubsystem->RemovePossibleTargetEntity(TargetEntity);
						Context.Defer().DestroyEntity(TargetEntity);
					}
				}				
			}
		}));
}
