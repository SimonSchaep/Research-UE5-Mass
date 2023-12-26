// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MassCommonTypes.h"
#include "UnitAnimState.h"
#include "UnitAnimInstance.generated.h"


UCLASS()
class BATTLESIMULATORMASS_API UUnitAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EUnitAnimState UnitAnimState;

private:
	bool TryGetEntityHandle();

	FMassEntityHandle EntityHandle;

};
