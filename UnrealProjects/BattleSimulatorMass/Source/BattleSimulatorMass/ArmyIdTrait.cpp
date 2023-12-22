// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmyIdTrait.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UArmyIdTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FArmyIdFragment>();
}
