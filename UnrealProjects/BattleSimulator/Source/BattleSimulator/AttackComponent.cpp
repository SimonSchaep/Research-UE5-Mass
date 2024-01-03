// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackComponent.h"
#include "TargetAcquisitionComponent.h"
#include "HealthComponent.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	RangeSqr = Range * Range;
}


// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	TargetAcquisitionComponent = GetOwner()->GetComponentByClass<UTargetAcquisitionComponent>();

	AttackDelayTimer = AttackDelay;
	
}


// Called every frame
void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TargetAcquisitionComponent->GetClosestTarget() != nullptr && TargetAcquisitionComponent->GetClosestTargetDistanceSqr() <= RangeSqr)
	{
		if (AttackDelayTimer > 0)
		{
			AttackDelayTimer -= DeltaTime;

			if (AttackDelayTimer <= AnimationAttackDelay)
			{
				bIsAttacking = true;
			}

			if (AttackDelayTimer > 0) return;
		}

		AttackTarget();
		AttackDelayTimer += AttackDelay;
		bIsAttacking = false;
	}
	else
	{
		if (AttackDelayTimer > AnimationAttackDelay)
		{
			AttackDelayTimer -= DeltaTime;
		}
		else
		{
			AttackDelayTimer = AnimationAttackDelay;
		}
		bIsAttacking = false;
	}
}

bool UAttackComponent::IsAttacking() const
{
	return bIsAttacking;
}

void UAttackComponent::AttackTarget()
{	
	TargetAcquisitionComponent->GetClosestTarget()->GetComponentByClass<UHealthComponent>()->DoDamage(Damage);
}

