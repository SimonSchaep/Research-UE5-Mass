// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassTranslator.h"
#include "UnitTranslators.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API UUnitTransformTranslator : public UMassTranslator
{
	GENERATED_BODY()

public:
	UUnitTransformTranslator();

protected:
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context) override;

	FMassEntityQuery EntityQuery;
};

