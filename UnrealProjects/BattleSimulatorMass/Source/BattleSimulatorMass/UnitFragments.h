// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "UnitAnimState.h"
#include "UnitFragments.generated.h"

/**
 * 
 */
USTRUCT()
struct BATTLESIMULATORMASS_API FArmyIdFragment : public FMassFragment
{
	GENERATED_BODY()

	int ArmyId = 0;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitHealthFragment : public FMassFragment
{
	GENERATED_BODY()

	float CurrentHealth = 100.f;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitAttackFragment : public FMassFragment
{
	GENERATED_BODY()

	float AttackDelayTimer = 0.f;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitTargetAcquisitionFragment : public FMassFragment
{
	GENERATED_BODY()

	FMassEntityHandle CurrentTarget;

	float ClosestTargetDistanceSqr;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitAnimStateFragment : public FMassFragment
{
	GENERATED_BODY()

	EUnitAnimState UnitAnimState;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitAttackParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		float Range = 200.f;

	UPROPERTY(EditAnywhere)
		float AttackDelay = 1.f;

	UPROPERTY(EditAnywhere)
		float Damage = 20.f;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitMoveParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		float StopDistance = 100.f;
};