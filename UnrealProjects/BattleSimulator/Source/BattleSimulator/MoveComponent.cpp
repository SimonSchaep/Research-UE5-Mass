// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "TargetAcquisitionComponent.h"
#include "AIController.h"

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	StopRangeSqr = StopRange * StopRange;
}


// Called when the game starts
void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	TargetAcquisitionComponent = GetOwner()->GetComponentByClass<UTargetAcquisitionComponent>();

	AIController = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController());
}


// Called every frame
void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TargetAcquisitionComponent->GetClosestTarget() != nullptr && TargetAcquisitionComponent->GetClosestTargetDistanceSqr() > StopRangeSqr)
	{
		//AIController->MoveToLocation(TargetAcquisitionComponent->GetClosestTarget()->GetActorLocation(), StopRange);
		//AIController->;
		FVector Direction = (TargetAcquisitionComponent->GetClosestTarget()->GetActorLocation() - GetOwner()->GetActorLocation()).GetUnsafeNormal2D();
		GetOwner()->AddActorWorldOffset(Direction * MoveSpeed * DeltaTime, true);
	}
}

