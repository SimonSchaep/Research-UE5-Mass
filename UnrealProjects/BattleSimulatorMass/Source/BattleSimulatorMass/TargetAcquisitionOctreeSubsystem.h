// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MassEntityTypes.h"
#include "Spatial/SparseDynamicOctree3.h"
#include "Math/GenericOctree.h"
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

	const TArray<TUniquePtr<UE::Geometry::FSparseDynamicOctree3>>& GetOctrees()const;
	const TMap<int32, FMassEntityHandle>& GetObjectIDToEntityHandleMap()const;

private:
	FVector Origin = FVector(0, 0, 0);
	double Extent = 40000.0;

	//TOctree2<FMassEntityHandle, octreesemantics> oct;

	TArray<int> UnitCounts;

	int32 NextObjectId = 0;

	TMap<int32, FMassEntityHandle> ObjectIDToEntityHandleMap;

	FMassEntityManager* EntityManager;

	TArray<TUniquePtr<UE::Geometry::FSparseDynamicOctree3>> Octrees;
};
