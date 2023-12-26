// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitVisualizationTrait.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UUnitVisualizationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct VisualizationParamsFragment = EntityManager.GetOrCreateConstSharedFragment(VisualizationParameters);
	BuildContext.AddConstSharedFragment(VisualizationParamsFragment);
}
