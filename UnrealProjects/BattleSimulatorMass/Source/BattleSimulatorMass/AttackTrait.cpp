// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTrait.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UAttackTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(World);

	const FConstSharedStruct AttackParamsFragment = EntityManager.GetOrCreateConstSharedFragment(AttackParameters);
	BuildContext.AddConstSharedFragment(AttackParamsFragment);

	FUnitAttackFragment& AttackTemplate = BuildContext.AddFragment_GetRef<FUnitAttackFragment>();
	AttackTemplate.AttackDelayTimer = AttackParameters.AttackDelay;
}
