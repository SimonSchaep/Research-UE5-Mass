// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "TargetAcquisitionComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	if (TargetAcquisitionComponent->GetClosestTarget() == nullptr)
	{
		bIsRunning = false;
		return;
	}

	FVector Direction = (TargetAcquisitionComponent->GetClosestTarget()->GetActorLocation() - GetOwner()->GetActorLocation()).GetUnsafeNormal2D();

	if (TargetAcquisitionComponent->GetClosestTargetDistanceSqr() > StopRangeSqr)
	{
		auto Result = AIController->MoveToLocation(TargetAcquisitionComponent->GetClosestTarget()->GetActorLocation(), StopRange);
		GetOwner()->SetActorRotation(Direction.Rotation());
		bIsRunning = true;
	}
	else
	{
		bIsRunning = false;
	}
}

bool UMoveComponent::GetIsRunning() const
{
	return bIsRunning;
}
