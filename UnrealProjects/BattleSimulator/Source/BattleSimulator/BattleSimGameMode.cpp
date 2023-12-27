// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSimGameMode.h"
#include "UnitManager.h"
#include "Kismet/GameplayStatics.h"

ABattleSimGameMode::ABattleSimGameMode()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	UnitManager = CreateDefaultSubobject<UUnitManager>(TEXT("UnitManager"));
}

void ABattleSimGameMode::StartPlay()
{
	Super::StartPlay();
}

void ABattleSimGameMode::StartSpawning()
{
	if (bHasSpawned) return;

	bHasSpawned = true;

	UnitManager->SpawnUnits();
}

void ABattleSimGameMode::StartSimulation()
{
	if (bHasStartedSimulation) return;

	bHasStartedSimulation = true;

	OnStartSimulation.Broadcast();
}

void ABattleSimGameMode::Restart()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

bool ABattleSimGameMode::HasSpawned() const
{
	return bHasSpawned;
}

bool ABattleSimGameMode::HasStartedSimulation() const
{
	return bHasStartedSimulation;
}

UUnitManager* ABattleSimGameMode::GetUnitManager() const
{
	return UnitManager;
}