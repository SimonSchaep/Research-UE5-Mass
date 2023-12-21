// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MassEntityTypes.h"
#include "TargetAcquisitionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API UTargetAcquisitionSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UTargetAcquisitionSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void AddPossibleTargetEntity(const FMassEntityHandle& Handle, int ArmyId);

	const TArray<TArray<FMassEntityHandle>>& GetPossibleTargetEntities(int ArmyId)const;

private:
	TArray<TArray<FMassEntityHandle>> PossibleTargetEntities;
};
