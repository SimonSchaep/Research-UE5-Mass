// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathHandlingTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UDeathHandlingTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct DeathParamsFragment = EntityManager.GetOrCreateConstSharedFragment(DeathParameters);
	BuildContext.AddConstSharedFragment(DeathParamsFragment);

	FUnitDeathFragment& DeathTemplate = BuildContext.AddFragment_GetRef<FUnitDeathFragment>();
	DeathTemplate.DeathDelayTimer = DeathParameters.AnimationDeathDelay;
}