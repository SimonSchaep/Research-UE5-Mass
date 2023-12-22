// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveTrait.h"
#include "MassNavigationFragments.h"
#include "MassEntityTemplateRegistry.h"

void UMoveTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.AddFragment<FMassMoveTargetFragment>();
}