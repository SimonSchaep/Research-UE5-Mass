// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionSubsystem.h"
#include "MassSimulationSubsystem.h"

UTargetAcquisitionSubsystem::UTargetAcquisitionSubsystem()
{

}

TStatId UTargetAcquisitionSubsystem::GetStatId() const
{
	return TStatId();
}

void UTargetAcquisitionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UMassSimulationSubsystem>();
}

void UTargetAcquisitionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Disabling tick doesn't seem to work, so we use this return instead
#ifdef ENABLE_SPATIAL
	return;
#endif // ENABLE_SPATIAL

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Unit Count:")), false);
		for (int32 i{}; i < PossibleTargetEntities.Num(); ++i)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Army %i: %i"), i, PossibleTargetEntities[i].Num()), false);
		}		
	}
}

void UTargetAcquisitionSubsystem::AddPossibleTargetEntity(const FMassEntityHandle& Handle, int ArmyId)
{
	//Add armies until armyid, to ensure we don't go out of bounds
	while (ArmyId >= PossibleTargetEntities.Num())
	{
		PossibleTargetEntities.Add(TArray<FMassEntityHandle>());
	}
	PossibleTargetEntities[ArmyId].Add(Handle);
}

void UTargetAcquisitionSubsystem::RemovePossibleTargetEntity(const FMassEntityHandle& Handle, int ArmyId)
{
	PossibleTargetEntities[ArmyId].Remove(Handle);
}

const TArray<TArray<FMassEntityHandle>>& UTargetAcquisitionSubsystem::GetPossibleTargetEntities() const
{
	return PossibleTargetEntities;
}