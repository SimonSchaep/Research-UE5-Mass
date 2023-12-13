// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSim.h"
#include "UnitManager.h"

ABattleSim::ABattleSim()
{
	UnitManager = CreateDefaultSubobject<UUnitManager>(TEXT("UnitManager"));
}

UUnitManager* ABattleSim::GetUnitManager() const
{
	return UnitManager;
}
