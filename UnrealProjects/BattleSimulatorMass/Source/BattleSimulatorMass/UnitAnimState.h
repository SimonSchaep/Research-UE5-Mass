// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitAnimState.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EUnitAnimState : uint8
{
	Idle,
	Moving,
	Attacking,
	Dead,
};