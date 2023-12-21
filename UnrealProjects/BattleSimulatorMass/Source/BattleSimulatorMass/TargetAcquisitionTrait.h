// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "TargetAcquisitionTrait.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API UTargetAcquisitionTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

private:
	UPROPERTY(EditAnywhere, Category = "Mass|Attack")
		float Range = 200;

	UPROPERTY(EditAnywhere, Category = "Mass|Attack")
		float AttackDelay = 1.f;

	UPROPERTY(EditAnywhere, Category = "Mass|Attack")
		float Damage = 20;
};
