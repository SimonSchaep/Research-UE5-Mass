// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveTrait.h"
#include "MassNavigationFragments.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"

void UMoveTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct MoveParamsFragment = EntityManager.GetOrCreateConstSharedFragment(MoveParameters);
	BuildContext.AddConstSharedFragment(MoveParamsFragment);
}