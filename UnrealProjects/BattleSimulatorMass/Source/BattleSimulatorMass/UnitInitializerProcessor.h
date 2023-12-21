// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "UnitInitializerProcessor.generated.h"


//Based on UMassSpawnLocationProcessor

UCLASS()
class BATTLESIMULATORMASS_API UUnitInitializerProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UUnitInitializerProcessor();

protected:
	virtual void Initialize(UObject& Owner)override;
	virtual void ConfigureQueries()override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)override;

private:
	FMassEntityQuery EntityQuery;

	class UTargetAcquisitionSubsystem* TargetAcquisitionSubsystem;
};
