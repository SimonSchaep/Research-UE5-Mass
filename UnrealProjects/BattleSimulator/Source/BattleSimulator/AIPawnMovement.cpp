// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPawnMovement.h"

UAIPawnMovement::UAIPawnMovement()
{
	PrimaryComponentTick.bCanEverTick = true;

	MaxSpeed = 500.f;
}

void UAIPawnMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	const AController* Controller = PawnOwner->GetController();
	if (Controller && Controller->IsLocalController())
	{
		//Limit max speed
		if (IsExceedingMaxSpeed(MaxSpeed) == true)
		{
			Velocity = Velocity.GetUnsafeNormal() * MaxSpeed;
		}

		//Move actor
		FVector Delta = Velocity * DeltaTime;
		if (!Delta.IsNearlyZero(1e-6f))
		{
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			const FQuat Rotation = Delta.ToOrientationQuat();

			FHitResult Hit{};
			SafeMoveUpdatedComponent(Delta, Rotation, true, Hit);
		}

		//Needs to be done for movecomponent to work
		UpdateComponentVelocity();
	}
}

