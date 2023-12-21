// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitInitializerProcessor.h"
#include "UnitFragments.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassExecutionContext.h"
#include "MassDebugVisualizationComponent.h"
#include "MassGameplayDebugTypes.h"
#include "MassObserverRegistry.h"
#include "MassEQSSpawnDataGenerator.h"
#include "TargetAcquisitionSubsystem.h"


UUnitInitializerProcessor::UUnitInitializerProcessor()
	:EntityQuery(*this)
{
	bAutoRegisterWithProcessingPhases = false;
}

void UUnitInitializerProcessor::Initialize(UObject& Owner)
{
	Super::Initialize(Owner);

	TargetAcquisitionSubsystem = UWorld::GetSubsystem<UTargetAcquisitionSubsystem>(Owner.GetWorld());
}

void UUnitInitializerProcessor::ConfigureQueries()
{
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddRequirement<FArmyIdFragment>(EMassFragmentAccess::ReadWrite);
}

void UUnitInitializerProcessor::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	if (!ensure(Context.ValidateAuxDataType<FMassEntitySpawnData>()))
	{
		UE_VLOG_UELOG(this, LogMass, Log, TEXT("Execution context has invalid AuxData or it's not FMassSpawnAuxData. Entity transforms won't be initialized."));
		return;
	}

	const UWorld* World = EntityManager.GetWorld();
	check(World);

	const ENetMode NetMode = World->GetNetMode();

	FMassEntitySpawnData& AuxData = Context.GetMutableAuxData().GetMutable<FMassEntitySpawnData>();
	TArray<FTransform>& Transforms = AuxData.Transforms;
	int& UnitArmyId = AuxData.SpawnVars.ArmyId;

	const int32 NumSpawnTransforms = Transforms.Num();
	if (NumSpawnTransforms == 0)
	{
		UE_VLOG_UELOG(this, LogMass, Error, TEXT("No spawn transforms provided. Entity transforms won't be initialized."));
		return;
	}

	int32 NumRequiredSpawnTransforms = 0;
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [&NumRequiredSpawnTransforms](const FMassExecutionContext& Context)
		{
			NumRequiredSpawnTransforms += Context.GetNumEntities();
		});

	const int32 NumToAdd = NumRequiredSpawnTransforms - NumSpawnTransforms;
	if (NumToAdd > 0)
	{
		UE_VLOG_UELOG(this, LogMass, Warning,
			TEXT("Not enough spawn locations provided (%d) for all entities (%d). Existing locations will be reused randomly to fill the %d missing positions."),
			NumSpawnTransforms, NumRequiredSpawnTransforms, NumToAdd);

		Transforms.AddUninitialized(NumToAdd);
		for (int i = 0; i < NumToAdd; ++i)
		{
			Transforms[NumSpawnTransforms + i] = Transforms[FMath::RandRange(0, NumSpawnTransforms - 1)];
		}
	}

	if (AuxData.bRandomize)
	{
		EntityQuery.ForEachEntityChunk(EntityManager, Context, [&Transforms, &UnitArmyId, this](FMassExecutionContext& Context)
			{
				const TArrayView<FTransformFragment> LocationList = Context.GetMutableFragmentView<FTransformFragment>();
				const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
				const int32 NumEntities = Context.GetNumEntities();
				for (int32 i = 0; i < NumEntities; ++i)
				{
					//Spawn Transform
					const int32 AuxIndex = FMath::RandRange(0, Transforms.Num() - 1);
					LocationList[i].GetMutableTransform() = Transforms[AuxIndex];
					Transforms.RemoveAtSwap(AuxIndex, 1, /*bAllowShrinking=*/false);

					//Army Id
					ArmyIdList[i].ArmyId = UnitArmyId;

					//Add to subsystem
					TargetAcquisitionSubsystem->AddPossibleTargetEntity(Context.GetEntity(i), UnitArmyId);
				}
			});
	}
	else
	{
		int32 NextTransformIndex = 0;
		EntityQuery.ForEachEntityChunk(EntityManager, Context, [&Transforms, &NextTransformIndex, &UnitArmyId, this](FMassExecutionContext& Context)
			{
				//Spawn Transform
				const int32 NumEntities = Context.GetNumEntities();
				TArrayView<FTransformFragment> LocationList = Context.GetMutableFragmentView<FTransformFragment>();
				check(NextTransformIndex + NumEntities <= Transforms.Num());

				FMemory::Memcpy(LocationList.GetData(), &Transforms[NextTransformIndex], NumEntities * LocationList.GetTypeSize());
				NextTransformIndex += NumEntities;

				//Army Id
				const TArrayView<FArmyIdFragment> ArmyIdList = Context.GetMutableFragmentView<FArmyIdFragment>();
				for (int32 i = 0; i < Context.GetNumEntities(); ++i)
				{
					ArmyIdList[i].ArmyId = UnitArmyId;

					//Add to subsystem
					TargetAcquisitionSubsystem->AddPossibleTargetEntity(Context.GetEntity(i), UnitArmyId);
				}
			});
	}
}
