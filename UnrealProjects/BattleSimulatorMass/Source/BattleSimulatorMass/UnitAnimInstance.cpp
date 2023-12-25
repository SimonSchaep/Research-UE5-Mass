// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitAnimInstance.h"
#include "MassAgentComponent.h"
#include "UnitFragments.h"

void UUnitAnimInstance::NativeInitializeAnimation()
{
	TryGetEntityHandle();
}

void UUnitAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	FMassEntityManager* EntityManager = UE::Mass::Utils::GetEntityManager(GetWorld());
	if (EntityManager == nullptr) return;

	if (!EntityManager->IsEntityValid(EntityHandle))
	{
		if (!TryGetEntityHandle())
		{
			return;
		}
	}

	//Get animation state from fragment
	auto DataStruct = EntityManager->GetFragmentDataStruct(EntityHandle, FUnitAnimStateFragment::StaticStruct());
	if (!DataStruct.IsValid()) return;
	const FUnitAnimStateFragment& UnitAnimStateFragment = DataStruct.Get<FUnitAnimStateFragment>();
	UnitAnimState = UnitAnimStateFragment.UnitAnimState;
}

bool UUnitAnimInstance::TryGetEntityHandle()
{
	if (AActor* Owner = GetOwningActor())
	{
		if (UMassAgentComponent* MassAgent = Owner->GetComponentByClass<UMassAgentComponent>())
		{
			FMassEntityManager* EntityManager = UE::Mass::Utils::GetEntityManager(GetWorld());
			EntityHandle = MassAgent->GetEntityHandle();
			return EntityManager->IsEntityValid(EntityHandle);
		}
	}

	return false;
}
