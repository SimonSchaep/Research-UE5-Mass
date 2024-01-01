// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassProcessor.h"
#include "AttackProcessor.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API UAttackProcessor : public UMassProcessor
{
	GENERATED_BODY()
	
public:
	UAttackProcessor();

protected:
	virtual void Initialize(UObject& Owner)override;
	virtual void ConfigureQueries() override;
	virtual void Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)override;

private:
	FMassEntityQuery EntityQuery;

	class ABattleSimGameMode* GameMode;

	FCriticalSection Mutex;
};
