// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetAcquisitionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLESIMULATOR_API UTargetAcquisitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetAcquisitionComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AActor* GetClosestTarget()const;
	float GetClosestTargetDistanceSqr()const;

	UFUNCTION(BlueprintCallable)
		int GetTeamId()const;

private:
	void FindClosestTarget();

	class UUnitManager* UnitManager;

	AActor* ClosestTarget;

	float ClosestTargetDistanceSqr = FLT_MAX;

	int TeamId;
		
};
