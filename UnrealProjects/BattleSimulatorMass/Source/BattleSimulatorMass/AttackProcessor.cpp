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
	EntityQuery.AddRequirement<FUnitAnimStateFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FUnitAttackFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FUnitTargetAcquisitionFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddConstSharedRequirement<FUnitAttackParameters>(EMassFragmentPresence::All);
	EntityQuery.AddConstSharedRequirement<FUnitAnimParameters>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::None);
}

void UAttackProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, ([&](FMassExecutionContext& Context)
		{
			const TArrayView<FUnitAnimStateFragment> AnimStateList = Context.GetMutableFragmentView<FUnitAnimStateFragment>();
			const TArrayView<FUnitAttackFragment> AttackList = Context.GetMutableFragmentView<FUnitAttackFragment>();
			const TConstArrayView<FUnitTargetAcquisitionFragment> TargetAcquisitionList = Context.GetFragmentView<FUnitTargetAcquisitionFragment>();
			const FUnitAttackParameters& AttackParams = Context.GetConstSharedFragment<FUnitAttackParameters>();
			const FUnitAnimParameters& AnimParams = Context.GetConstSharedFragment<FUnitAnimParameters>();
			const float WorldDeltaTime = Context.GetDeltaTimeSeconds();

			for (int32 EntityIndex = 0; EntityIndex < Context.GetNumEntities(); ++EntityIndex)
			{
				if (TargetAcquisitionList[EntityIndex].ClosestTargetDistanceSqr <= FMath::Square(AttackParams.Range))
				{
					//Timer
					if (AttackList[EntityIndex].AttackDelayTimer > 0)
					{
						AttackList[EntityIndex].AttackDelayTimer -= WorldDeltaTime;

						if (AttackList[EntityIndex].AttackDelayTimer <= AnimParams.AnimationAttackDelay)
						{
							if (AnimStateList[EntityIndex].UnitAnimState != EUnitAnimState::Dead)
							{
								AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Attacking;
							}
						}

						if (AttackList[EntityIndex].AttackDelayTimer > 0) continue;
					}

					//Attack
					AttackList[EntityIndex].AttackDelayTimer += AttackParams.AttackDelay;

					//Get health fragment
					const FMassEntityHandle& TargetEntity = TargetAcquisitionList[EntityIndex].CurrentTarget;
					if (!TargetEntity.IsValid()) continue;
					auto HealthDataStruct = EntityManager.GetFragmentDataStruct(TargetEntity, FUnitHealthFragment::StaticStruct());
					if (!HealthDataStruct.IsValid()) continue;
					FUnitHealthFragment& TargetEntityHealth = HealthDataStruct.Get<FUnitHealthFragment>();

					//Do damage
					TargetEntityHealth.CurrentHealth -= AttackParams.Damage;

					//Check if dead
					if (TargetEntityHealth.CurrentHealth <= 0)
					{
						TargetAcquisitionSubsystem->RemovePossibleTargetEntity(TargetEntity);
						EntityManager.Defer().AddTag<FDyingTag>(TargetEntity);

						auto AnimDataStruct = EntityManager.GetFragmentDataStruct(TargetEntity, FUnitAnimStateFragment::StaticStruct());
						if (!AnimDataStruct.IsValid()) continue;
						FUnitAnimStateFragment& TargetEntityAnimState = AnimDataStruct.Get<FUnitAnimStateFragment>();
						TargetEntityAnimState.UnitAnimState = EUnitAnimState::Dead;
					}

					//We are done attacking, so set anim state to idle (if we didn't die this frame)
					if (AnimStateList[EntityIndex].UnitAnimState != EUnitAnimState::Dead)
					{
						AnimStateList[EntityIndex].UnitAnimState = EUnitAnimState::Idle;
					}
				}
				else
				{
					//Reset attack delay timer
					AttackList[EntityIndex].AttackDelayTimer = AnimParams.AnimationAttackDelay;
				}
			}
		}));
}
