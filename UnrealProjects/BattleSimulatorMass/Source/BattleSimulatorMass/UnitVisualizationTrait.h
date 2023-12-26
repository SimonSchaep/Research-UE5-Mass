// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "UnitFragments.h"
#include "UnitVisualizationTrait.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API UUnitVisualizationTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

private:
	UPROPERTY(EditAnywhere)
	FUnitVisualizationParameters VisualizationParameters;
};
