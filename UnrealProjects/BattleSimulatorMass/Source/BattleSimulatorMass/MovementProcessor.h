// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "MovementProcessor.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API UMovementProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UMovementProcessor();

protected:
	virtual void Initialize(UObject& Owner)override;
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)override;

private:
	FMassEntityQuery EntityQuery;

	class ABattleSimGameMode* GameMode;

	class UNavigationSystemV1* NavigationSystem;
};
