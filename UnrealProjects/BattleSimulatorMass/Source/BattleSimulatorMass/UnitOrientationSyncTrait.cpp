// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitOrientationSyncTrait.h"
#include "UnitTags.h"
#include "MassEntityTemplateRegistry.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "Translators/MassSceneComponentLocationTranslator.h"

void UUnitOrientationSyncTrait::BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const
{
	BuildContext.RequireFragment<FTransformFragment>();
	BuildContext.RequireFragment<FMassSceneComponentWrapperFragment>();

	BuildContext.AddTag<FSyncRotationTag>();
}
