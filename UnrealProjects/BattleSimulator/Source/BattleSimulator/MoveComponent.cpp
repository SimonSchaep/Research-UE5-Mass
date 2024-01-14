// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveComponent.h"
#include "TargetAcquisitionComponent.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

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
		AIController->StopMovement();
		bIsMoving = false;
		return;
	}

	FVector Direction = (TargetAcquisitionComponent->GetClosestTarget()->GetActorLocation() - GetOwner()->GetActorLocation()).GetUnsafeNormal2D();

	if (TargetAcquisitionComponent->GetClosestTargetDistanceSqr() > StopRangeSqr)
	{
		//Try to move to target actor
		if (AIController->MoveToActor(TargetAcquisitionComponent->GetClosestTarget(), 0) == EPathFollowingRequestResult::Type::Failed)
		{
			//If failed -> we are likely outside the navmesh -> move to the closest point on the navmesh instead
			AIController->MoveToLocation(AIController->GetNavAgentLocation());
		}
		bIsMoving = true;
	}
	else
	{
		//Stop moving, but still rotate to our target
		AIController->StopMovement();
		FRotator Rotation = FRotator(0, Direction.ToOrientationRotator().Yaw, 0);
		GetOwner()->SetActorRotation(Rotation);
		bIsMoving = false;
	}
}

bool UMoveComponent::IsMoving() const
{
	return bIsMoving;
}
