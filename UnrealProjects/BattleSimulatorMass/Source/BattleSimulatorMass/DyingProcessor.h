// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "DyingProcessor.generated.h"

// Processor that handles units that are dying, removes most of their logic, and spawns new "dead" entities to replace them

UCLASS()
class BATTLESIMULATORMASS_API UDyingProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UDyingProcessor();

protected:
	virtual void Initialize(UObject& Owner)override;
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)override;

private:
	FMassEntityQuery EntityQuery;

	class UMassSpawnerSubsystem* SpawnerSubsystem;
	
#ifdef ENABLE_SPATIAL
	class UTargetAcquisitionOctreeSubsystem* TargetAcquisitionSubsystem;
#else
	class UTargetAcquisitionSubsystem* TargetAcquisitionSubsystem;
#endif // ENABLE_SPATIAL
};
