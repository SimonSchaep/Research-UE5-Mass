// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimationTrait.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UUnitAnimationTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct AnimParamsFragment = EntityManager.GetOrCreateConstSharedFragment(AnimationParameters);
	BuildContext.AddConstSharedFragment(AnimParamsFragment);

	BuildContext.AddFragment<FUnitAnimStateFragment>();
}