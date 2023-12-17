// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AIPawnMovement.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLESIMULATOR_API UAIPawnMovement : public UPawnMovementComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIPawnMovement();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual float GetMaxSpeed() const override { return MaxSpeed; }

private:
	UPROPERTY(EditAnywhere)
		float MaxSpeed;		
};
