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

	//Get all mass spawners, sadly we can't directly get them as AMassSpawner
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMassSpawner::StaticClass(), FoundActors);
	for (auto Actor : FoundActors)
	{
		ArmySpawners.Add(Cast<AMassSpawner>(Actor));
	}
}

void ABattleSimGameMode::StartSpawning()
{
	for (auto Spawner : ArmySpawners)
	{
		Spawner->DoSpawning();
	}
}

void ABattleSimGameMode::Restart()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}