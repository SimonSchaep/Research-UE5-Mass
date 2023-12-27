// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UnitAnimState.h"
#include "UnitAnimInstance.generated.h"


UCLASS()
class BATTLESIMULATOR_API UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EUnitAnimState UnitAnimState;

private:
	void GetComponentsFromOwner();

	class UAttackComponent* AttackComponent;
	class UHealthComponent* HealthComponent;
	class UMoveComponent* MoveComponent;
};
