// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionComponent.h"
#include "UnitManager.h"
#include "BattleSim.h"

// Sets default values for this component's properties
UTargetAcquisitionComponent::UTargetAcquisitionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargetAcquisitionComponent::BeginPlay()
{
	Super::BeginPlay();

	ABattleSim* GameMode = Cast<ABattleSim>(GetWorld()->GetAuthGameMode());
	UnitManager = GameMode->GetUnitManager();

	TeamId = UnitManager->GetNextTeamId();
}


// Called every frame
void UTargetAcquisitionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FindClosestTarget();
}

AActor* UTargetAcquisitionComponent::GetClosestTarget() const
{
	return ClosestTarget;
}

float UTargetAcquisitionComponent::GetClosestTargetDistanceSqr() const
{
	return ClosestTargetDistanceSqr;
}

int UTargetAcquisitionComponent::GetTeamId() const
{
	return TeamId;
}

void UTargetAcquisitionComponent::FindClosestTarget()
{
	ClosestTargetDistanceSqr = FLT_MAX;
	float DistanceSqr;
	for (AActor* Actor : UnitManager->GetAllEnemyUnits(TeamId))
	{
		DistanceSqr = FVector::DistSquared(GetOwner()->GetActorLocation(), Actor->GetActorLocation());
		if (DistanceSqr < ClosestTargetDistanceSqr)
		{
			ClosestTarget = Actor;
			ClosestTargetDistanceSqr = DistanceSqr;
		}
	}
}

