// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimStateTrait.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UUnitAnimStateTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FUnitAnimStateFragment>();
}
