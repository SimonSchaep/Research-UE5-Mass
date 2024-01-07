// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLESIMULATOR_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttackComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsAttacking()const;

private:
	void AttackTarget();

	bool bIsAttacking = false;

	class UTargetAcquisitionComponent* TargetAcquisitionComponent;

	float AttackDelayTimer = 0.f;

	float RangeSqr;

	UPROPERTY(EditAnywhere)
		float AttackDelay = 1.f;

	UPROPERTY(EditAnywhere)
		float AnimationAttackDelay = .4f;

	UPROPERTY(EditAnywhere)
		float Range = 200;

	UPROPERTY(EditAnywhere)
		float Damage = 20;
		
};
