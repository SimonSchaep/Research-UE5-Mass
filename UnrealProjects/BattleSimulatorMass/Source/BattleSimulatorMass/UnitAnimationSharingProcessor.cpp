// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimationSharingProcessor.h"
#include "UnitAnimState.h"
#include "MassCommonTypes.h"
#include "UnitFragments.h"
#include "MassAgentComponent.h"

UUnitAnimationSharingProcessor::UUnitAnimationSharingProcessor(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UUnitAnimationSharingProcessor::ProcessActorState_Internal(int32& OutState, AActor* InActor, uint8 CurrentState, uint8 OnDemandState, bool& bShouldProcess)
{
	FMassEntityHandle EntityHandle{};
	if (UMassAgentComponent* MassAgent = InActor->GetComponentByClass<UMassAgentComponent>())
	{
		EntityHandle = MassAgent->GetEntityHandle();
	}

	FMassEntityManager* EntityManager = UE::Mass::Utils::GetEntityManager(InActor->GetWorld());
	if (EntityManager == nullptr) return;

	if (!EntityManager->IsEntityValid(EntityHandle))
	{
		return;
	}

	//Get animation state from fragment
	auto DataStruct = EntityManager->GetFragmentDataStruct(EntityHandle, FUnitAnimStateFragment::StaticStruct());
	if (!DataStruct.IsValid()) return;
	const FUnitAnimStateFragment& UnitAnimStateFragment = DataStruct.Get<FUnitAnimStateFragment>();
	OutState = int32(UnitAnimStateFragment.UnitAnimState);

	bShouldProcess = true;
}
