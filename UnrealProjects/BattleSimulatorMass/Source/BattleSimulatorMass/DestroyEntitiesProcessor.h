// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "DestroyEntitiesProcessor.generated.h"

// Processor that handles destruction of entities

UCLASS()
class BATTLESIMULATORMASS_API UDestroyEntitiesProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UDestroyEntitiesProcessor();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)override;

private:
	FMassEntityQuery EntityQuery;
};
