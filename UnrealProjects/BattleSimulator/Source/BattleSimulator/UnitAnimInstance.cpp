// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimInstance.h"
#include "AttackComponent.h"
#include "HealthComponent.h"
#include "MoveComponent.h"

void UUnitAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	UnitAnimState = EUnitAnimState::Idle;

	GetComponentsFromOwner();
}

void UUnitAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (HealthComponent == nullptr || AttackComponent == nullptr || MoveComponent == nullptr)
	{
		GetComponentsFromOwner();
		return;
	}

	//Determine anim state
	if (HealthComponent->HasDied())
	{
		UnitAnimState = EUnitAnimState::Dead;
		return;
	}
	if (AttackComponent->IsAttacking())
	{
		UnitAnimState = EUnitAnimState::Attacking;
		return;
	}
	if (MoveComponent->IsMoving())
	{
		UnitAnimState = EUnitAnimState::Moving;
		return;
	}
	UnitAnimState = EUnitAnimState::Idle;
}

void UUnitAnimInstance::GetComponentsFromOwner()
{
	if (GetOwningActor() != nullptr)
	{
		AttackComponent = GetOwningActor()->GetComponentByClass<UAttackComponent>();
		HealthComponent = GetOwningActor()->GetComponentByClass<UHealthComponent>();
		MoveComponent = GetOwningActor()->GetComponentByClass<UMoveComponent>();
	}
}
