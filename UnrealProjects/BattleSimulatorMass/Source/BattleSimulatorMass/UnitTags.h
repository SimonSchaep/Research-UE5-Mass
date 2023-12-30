// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTypes.h"
#include "UnitTags.generated.h"

/**
 * 
 */
USTRUCT()
struct BATTLESIMULATORMASS_API FDyingTag : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT()
struct BATTLESIMULATORMASS_API FDeadTag : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT()
struct BATTLESIMULATORMASS_API FSyncRotationTag : public FMassTag
{
	GENERATED_BODY()
};

USTRUCT()
struct BATTLESIMULATORMASS_API FStandingStillTag : public FMassTag
{
	GENERATED_BODY()
};