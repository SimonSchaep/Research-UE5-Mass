// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleSimGameMode.h"
#include "MassSpawner.h"
#include "Kismet/GameplayStatics.h"

ABattleSimGameMode::ABattleSimGameMode()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ABattleSimGameMode::StartPlay()
{
	Super::StartPlay();

	bHasSpawned = false;
	bHasStartedSimulation = false;

	//Get all mass spawners
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMassSpawner::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		ArmySpawners.Add(Cast<AMassSpawner>(Actor));
	}
}

void ABattleSimGameMode::StartSpawning()
{
	if (bHasSpawned) return;

	bHasSpawned = true;
	for (auto Spawner : ArmySpawners)
	{
		Spawner->DoSpawning();
	}
}

void ABattleSimGameMode::StartSimulation()
{
	if (bHasStartedSimulation) return;

	bHasStartedSimulation = true;
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
