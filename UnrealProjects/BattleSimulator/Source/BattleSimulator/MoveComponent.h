// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MoveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLESIMULATOR_API UMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMoveComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		bool IsMoving()const;

private:
	class UTargetAcquisitionComponent* TargetAcquisitionComponent;

	class AAIController* AIController;

	class UNavigationSystemV1* NavigationSystem;

	float StopRangeSqr;

	bool bIsMoving = false;

	UPROPERTY(EditAnywhere)
		float StopRange = 150.f;
};
