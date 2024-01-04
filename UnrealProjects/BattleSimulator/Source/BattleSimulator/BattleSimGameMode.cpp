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
	UnitManager->SetMinBounds(FVector(
		UGameplayStatics::GetIntOption(OptionsString, L"MinBoundsX", 0),
		UGameplayStatics::GetIntOption(OptionsString, L"MinBoundsY", 0),
		0
	));
	UnitManager->SetMaxBounds(FVector(
		UGameplayStatics::GetIntOption(OptionsString, L"MaxBoundsX", 0),
		UGameplayStatics::GetIntOption(OptionsString, L"MaxBoundsY", 0),
		0
	));

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
		FString("?MinBoundsX=" + int(UnitManager->GetMinBounds().X)) +
		FString("?MinBoundsY=" + int(UnitManager->GetMinBounds().Y)) +
		FString("?MaxBoundsX=" + int(UnitManager->GetMaxBounds().X)) +
		FString("?MaxBoundsY=" + int(UnitManager->GetMaxBounds().Y))
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