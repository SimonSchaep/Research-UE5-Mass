// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthTrait.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UHealthTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FUnitHealthFragment& HealthTemplate = BuildContext.AddFragment_GetRef<FUnitHealthFragment>();
	HealthTemplate.CurrentHealth = Health;
}
