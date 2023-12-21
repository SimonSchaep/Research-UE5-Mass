// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionTrait.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"

void UTargetAcquisitionTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment_GetRef<FTargetAcquisitionFragment>();
}
