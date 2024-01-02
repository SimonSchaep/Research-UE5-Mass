// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationSharingTypes.h"
#include "UnitAnimationSharingProcessor.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API UUnitAnimationSharingProcessor : public UAnimationSharingStateProcessor
{
	GENERATED_BODY()

public:
	UUnitAnimationSharingProcessor(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void ProcessActorState_Internal(int32& OutState, AActor* InActor, uint8 CurrentState, uint8 OnDemandState, bool& bShouldProcess);

};
