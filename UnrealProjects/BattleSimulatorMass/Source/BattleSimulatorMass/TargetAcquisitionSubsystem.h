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
class BATTLESIMULATORMASS_API UTargetAcquisitionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UTargetAcquisitionSubsystem();

	//Pure virtual impl
	virtual TStatId GetStatId() const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;

	void AddPossibleTargetEntity(const FMassEntityHandle& Handle, int ArmyId);
	void RemovePossibleTargetEntity(const FMassEntityHandle& Handle, int ArmyId);

	const TArray<TArray<FMassEntityHandle>>& GetPossibleTargetEntities()const;

private:
	TArray<TArray<FMassEntityHandle>> PossibleTargetEntities;
};
