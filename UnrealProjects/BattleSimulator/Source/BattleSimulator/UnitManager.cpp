// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitManager.h"
#include "Kismet/GameplayStatics.h"
#include "BattleSimGameMode.h"

// Sets default values
UUnitManager::UUnitManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryComponentTick.bCanEverTick = true;

	Units.Add(TArray<AActor*>());
	Units.Add(TArray<AActor*>());

	Units[0].Reserve(SpawnCount/2);
	Units[1].Reserve(SpawnCount/2);
}

// Called when the game starts or when spawned
void UUnitManager::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnUnits();
}

// Called every frame
void UUnitManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Unit count: %i"), GetUnitCount()));

	if (Units[0].Num() == 0 || Units[1].Num() == 0)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
}

TArray<AActor*>& UUnitManager::GetAllEnemyUnits(int TeamId)
{
	return Units[TeamId == 0 ? 1 : 0];
}

void UUnitManager::UnRegisterUnit(AActor* Unit)
{
	Units[0].Remove(Unit);
	Units[1].Remove(Unit);
}

int UUnitManager::GetNextTeamId() const
{
	return NextTeamId;
}

void UUnitManager::SpawnUnits()
{
	FTransform SpawnTransform{};
	NextTeamId = 0;
	for (int i{}; i < SpawnCount / 2; ++i)
	{
		SpawnTransform.SetLocation(FVector(
			FMath::RandRange(-SpawnPosRange, SpawnPosRange),
			FMath::RandRange(SpawnPosRange/2, SpawnPosRange),
			0));
		SpawnUnit(SpawnTransform);
	}
	NextTeamId = 1;
	for (int i{}; i < SpawnCount / 2; ++i)
	{
		SpawnTransform.SetLocation(FVector(
			FMath::RandRange(-SpawnPosRange, SpawnPosRange),
			FMath::RandRange(-SpawnPosRange, -SpawnPosRange/2),
			0));
		SpawnUnit(SpawnTransform);
	}
}

void UUnitManager::SpawnUnit(const FTransform& SpawnTransform)
{
	AActor* SpawnedActor = GetWorld()->SpawnActor(UnitTemplate, &SpawnTransform);
	Units[NextTeamId].Add(SpawnedActor);
}

int UUnitManager::GetUnitCount() const
{
	return Units[0].Num() + Units[1].Num();
}

