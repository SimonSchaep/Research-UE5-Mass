// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionSubsystem.h"
#include "MassSimulationSubsystem.h"

UTargetAcquisitionSubsystem::UTargetAcquisitionSubsystem()
{
}

void UTargetAcquisitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UMassSimulationSubsystem>();
}

void UTargetAcquisitionSubsystem::AddPossibleTargetEntity(const FMassEntityHandle& Handle, int ArmyId)
{
	//Add armies until armyid
	while (ArmyId >= PossibleTargetEntities.Num())
	{
		PossibleTargetEntities.Add(TArray<FMassEntityHandle>());
	}
	PossibleTargetEntities[ArmyId].Add(Handle);
}

void UTargetAcquisitionSubsystem::RemovePossibleTargetEntity(const FMassEntityHandle& Handle)
{
	for (auto Array : PossibleTargetEntities)
	{
		Array.Remove(Handle);
	}
}

const TArray<TArray<FMassEntityHandle>>& UTargetAcquisitionSubsystem::GetPossibleTargetEntities() const
{
	return PossibleTargetEntities;
}