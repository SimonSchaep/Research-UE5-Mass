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

	UnitManager->SetSpawnCount(UGameplayStatics::GetIntOption(OptionsString, L"SpawnCount", 0));
	UnitManager->SetSpawnPosRange(UGameplayStatics::GetIntOption(OptionsString, L"SpawnPosRange", 0));

	StartSpawning();
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
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false, 
		FString("SpawnCount=" + UnitManager->GetSpawnCount()) + 
		FString("\nSpawnPosRange=" + UnitManager->GetSpawnPosRange())
	);
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