// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MassEntityTypes.h"
#include "UnitOctreeTypes.h"
#include "TargetAcquisitionOctreeSubsystem.generated.h"



UCLASS()
class BATTLESIMULATORMASS_API UTargetAcquisitionOctreeSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UTargetAcquisitionOctreeSubsystem();

	//Pure virtual impl
	virtual TStatId GetStatId() const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;

	//Requires Entity to have FUnitOctreeDataFragment
	void AddPossibleTargetEntity(const FMassEntityHandle& Entity, int ArmyId);
	void RemovePossibleTargetEntity(const FMassEntityHandle& Entity, int ArmyId);

	void UpdatePossibleTargetEntity(const FMassEntityHandle& Entity, int ArmyId);

	const TArray<FUnitOctree>& GetOctrees()const;

private:
	FVector Origin = FVector(0, 0, 0);
	double Extent = 1000000.0;

	TArray<int> UnitCounts;

	FMassEntityManager* EntityManager;

	TArray<FUnitOctree> Octrees;

	bool bDrawDebug = false;
};
