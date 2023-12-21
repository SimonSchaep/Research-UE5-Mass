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
	UnitManager->SpawnUnits();
}

void ABattleSimGameMode::Restart()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

UUnitManager* ABattleSimGameMode::GetUnitManager() const
{
	return UnitManager;
}