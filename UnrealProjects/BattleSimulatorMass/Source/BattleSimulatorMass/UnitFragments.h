// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "UnitAnimState.h"
#include "MassRepresentationTypes.h"
#include "MassCommonFragments.h"
#include "MassCommonTypes.h"
#include "MassSpawnerTypes.h"
#include "BoxTypes.h"
#include "UnitOctreeTypes.h"
#include "UnitFragments.generated.h"


//Fragments

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
struct BATTLESIMULATORMASS_API FUnitDeathFragment : public FMassFragment
{
	GENERATED_BODY()

	float DeathDelayTimer = 0.f;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitOctreeDataFragment : public FMassFragment
{
	GENERATED_BODY()

		//int32 ObjectId;

	FUnitOctreeDataFragment() : SharedOctreeId(MakeShareable(new FUnitOctreeId())) {}

	FUnitOctreeIdSharedRef SharedOctreeId;

	//FBoxSphereBounds Bounds;
};


//Shared Fragments

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitDeathParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		UMassEntityConfigAsset* DeadEntityConfig;

	UPROPERTY(EditAnywhere)
		float AnimationDeathDelay;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitAnimParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		float AnimationAttackDelay;
};

USTRUCT()
struct BATTLESIMULATORMASS_API FUnitVisualizationParameters : public FMassSharedFragment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AActor>> LowResTemplateActors;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<AActor>> HighResTemplateActors;

	UPROPERTY(EditAnywhere)
		TArray<FStaticMeshInstanceVisualizationDesc> StaticMeshInstanceDescs;
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