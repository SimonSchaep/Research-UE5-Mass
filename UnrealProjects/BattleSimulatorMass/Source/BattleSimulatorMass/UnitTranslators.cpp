// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitTranslators.h"
#include "MassCommonTypes.h"
#include "MassCommonFragments.h"
#include "MassExecutionContext.h"
#include "UnitFragments.h"
#include "UnitTags.h"
#include "Translators/MassSceneComponentLocationTranslator.h"

UUnitOrientationTranslator::UUnitOrientationTranslator()
	: EntityQuery{ *this }
{
	bAutoRegisterWithProcessingPhases = true;
	ExecutionFlags = (int32)EProcessorExecutionFlags::All;
	ExecutionOrder.ExecuteInGroup = UE::Mass::ProcessorGroupNames::UpdateWorldFromMass;
	ExecutionOrder.ExecuteAfter.Add(UE::Mass::ProcessorGroupNames::Movement);
	bRequiresGameThreadExecution = true;
}

void UUnitOrientationTranslator::ConfigureQueries()
{
	EntityQuery.AddRequirement<FMassSceneComponentWrapperFragment>(EMassFragmentAccess::ReadOnly);
	EntityQuery.AddRequirement<FTransformFragment>(EMassFragmentAccess::ReadWrite);
	EntityQuery.AddTagRequirement<FSyncRotationTag>(EMassFragmentPresence::All);
	EntityQuery.AddTagRequirement<FDeadTag>(EMassFragmentPresence::None);
	EntityQuery.AddTagRequirement<FDyingTag>(EMassFragmentPresence::None);
	EntityQuery.RequireMutatingWorldAccess();
}

void UUnitOrientationTranslator::Execute(FMassEntityManager& EntityManager, FMassExecutionContext& Context)
{
	EntityQuery.ForEachEntityChunk(EntityManager, Context, [this](FMassExecutionContext& Context)
		{
			const TConstArrayView<FMassSceneComponentWrapperFragment> SceneComponentList = Context.GetFragmentView<FMassSceneComponentWrapperFragment>();
			const TArrayView<FTransformFragment> TransformList = Context.GetMutableFragmentView<FTransformFragment>();

			const int32 NumEntities = Context.GetNumEntities();

			for (int32 EntityIndex = 0; EntityIndex < NumEntities; ++EntityIndex)
			{
				if (USceneComponent* SceneComponent = SceneComponentList[EntityIndex].Component.Get())
				{
					const FTransformFragment& Transform = TransformList[EntityIndex];
					SceneComponent->SetWorldRotation(Transform.GetTransform().GetRotation());
				}
			}
		});
}