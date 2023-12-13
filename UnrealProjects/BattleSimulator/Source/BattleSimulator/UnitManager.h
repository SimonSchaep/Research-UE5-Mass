// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitManager.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BATTLESIMULATOR_API UUnitManager : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UUnitManager();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<AActor*>& GetAllEnemyUnits(int TeamId);

	void UnRegisterUnit(AActor* Unit);

	int GetNextTeamId()const;

private:
	void SpawnUnits();
	void SpawnUnit(const FTransform& SpawnTransform);

	TArray<TArray<AActor*>> Units;

	UPROPERTY(EditAnywhere)
		int SpawnCount = 100;

	UPROPERTY(EditAnywhere)
		int SpawnPosRange = 10000;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> UnitTemplate;

	int NextTeamId = 0;

};
