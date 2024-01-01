// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetAcquisitionOctreeSubsystem.h"
#include "MassSimulationSubsystem.h"
#include "MassCommonTypes.h"
#include "MassEntityManager.h"
#include "MassEntityView.h"
#include "UnitFragments.h"


UTargetAcquisitionOctreeSubsystem::UTargetAcquisitionOctreeSubsystem()
{
}

TStatId UTargetAcquisitionOctreeSubsystem::GetStatId() const
{
	return TStatId();
}

void UTargetAcquisitionOctreeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency<UMassSimulationSubsystem>();

	EntityManager = UE::Mass::Utils::GetEntityManager(GetWorld());	
}

void UTargetAcquisitionOctreeSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Print unit counts
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Unit Count:")), false);
		for (int32 i{}; i < UnitCounts.Num(); ++i)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Army %i: %i"), i, UnitCounts[i]), false);
		}			
	}
}

void UTargetAcquisitionOctreeSubsystem::AddPossibleTargetEntity(const FMassEntityHandle& Entity, int ArmyId)
{
	//Add octrees until armyid
	while (ArmyId >= Octrees.Num())
	{
		int32 id = Octrees.Add(MakeUnique<UE::Geometry::FSparseDynamicOctree3>());
		Octrees[id]->RootDimension = 1000.f;
		Octrees[id]->SetMaxTreeDepth(5);

		UnitCounts.Add(0);
	}

	//Calculate bounds
	FMassEntityView View{ *EntityManager, Entity };
	FVector Center = View.GetFragmentData<FTransformFragment>().GetTransform().GetLocation();
	float Radius = View.GetFragmentData<FAgentRadiusFragment>().Radius;
	UE::Geometry::FAxisAlignedBox3d Bounds{Center, double(Radius)};

	//Insert object in Octree
	Octrees[ArmyId]->InsertObject(NextObjectId, Bounds);

	//Update FUnitOctreeDataFragment values
	View.GetFragmentData<FUnitOctreeDataFragment>().Bounds = Bounds;
	View.GetFragmentData<FUnitOctreeDataFragment>().ObjectId = NextObjectId;

	//Store to map
	ObjectIDToEntityHandleMap.Add(NextObjectId, Entity);


	++NextObjectId;
	++UnitCounts[ArmyId];
}

void UTargetAcquisitionOctreeSubsystem::RemovePossibleTargetEntity(const FMassEntityHandle& Entity, int ArmyId)
{
	FMassEntityView View = FMassEntityView(*EntityManager, Entity);
	
	int32 Id = View.GetFragmentData<FUnitOctreeDataFragment>().ObjectId;
	Octrees[ArmyId]->RemoveObject(Id);
	ObjectIDToEntityHandleMap.Remove(Id);

	--UnitCounts[ArmyId];
}

void UTargetAcquisitionOctreeSubsystem::UpdatePossibleTargetEntity(const FMassEntityHandle& Entity, int ArmyId)
{
	//Calculate bounds
	FMassEntityView View{ *EntityManager, Entity };
	FVector Center = View.GetFragmentData<FTransformFragment>().GetTransform().GetLocation();
	float Radius = View.GetFragmentData<FAgentRadiusFragment>().Radius;
	UE::Geometry::FAxisAlignedBox3d Bounds{Center, double(Radius)};

	int32 ObjectId = View.GetFragmentData<FUnitOctreeDataFragment>().ObjectId;

	uint32 CellId;

	//Reinsert if needed
	if (Octrees[ArmyId]->CheckIfObjectNeedsReinsert(ObjectId, Bounds, CellId))
	{
		Octrees[ArmyId]->ReinsertObject(ObjectId, Bounds, CellId);
	}

	View.GetFragmentData<FUnitOctreeDataFragment>().Bounds = Bounds;
}

const TArray<TUniquePtr<UE::Geometry::FSparseDynamicOctree3>>& UTargetAcquisitionOctreeSubsystem::GetOctrees() const
{
	return Octrees;
}

const TMap<int32, FMassEntityHandle>& UTargetAcquisitionOctreeSubsystem::GetObjectIDToEntityHandleMap() const
{
	return ObjectIDToEntityHandleMap;
}
