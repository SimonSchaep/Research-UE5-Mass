// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackTrait.h"
#include "UnitFragments.h"
#include "MassEntityTemplateRegistry.h"
#include "MassEntityTemplate.h"

void UAttackTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	FAttackFragment& AttackTemplate = BuildContext.AddFragment_GetRef<FAttackFragment>();
	AttackTemplate.AttackDelay = AttackDelay;
	AttackTemplate.AttackDelayTimer = AttackDelay;
	AttackTemplate.Damage = Damage;
	AttackTemplate.RangeSqr = Range * Range;
}
