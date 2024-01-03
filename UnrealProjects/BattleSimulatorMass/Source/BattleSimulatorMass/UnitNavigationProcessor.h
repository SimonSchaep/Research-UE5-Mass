// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "UnitNavigationProcessor.generated.h"


// Processor that finds a navigationpath towards the acquired target

UCLASS()
class BATTLESIMULATORMASS_API UUnitNavigationProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UUnitNavigationProcessor();

protected:
	virtual void Initialize(UObject& Owner)override;
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)override;

private:
	FMassEntityQuery EntityQuery;

	class ABattleSimGameMode* GameMode;

	class UNavigationSystemV1* NavigationSystem;
};
