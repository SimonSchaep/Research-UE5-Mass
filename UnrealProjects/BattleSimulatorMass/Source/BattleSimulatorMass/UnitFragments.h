// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "UnitFragments.generated.h"

/**
 * 
 */
USTRUCT()
struct BATTLESIMULATORMASS_API FArmyIdFragment : public FMassFragment
{
	GENERATED_BODY()

	int ArmyId;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FHealthFragment : public FMassFragment
{
	GENERATED_BODY()

	float CurrentHealth;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FAttackFragment : public FMassFragment
{
	GENERATED_BODY()

	float AttackDelayTimer = 0.f;

	float RangeSqr;

	float AttackDelay;

	float Damage;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FTargetAcquisitionFragment : public FMassFragment
{
	GENERATED_BODY()

	FMassEntityHandle CurrentTarget;

	float ClosestTargetDistanceSqr = FLT_MAX;
};