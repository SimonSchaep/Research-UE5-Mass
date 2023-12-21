// Fill out your copyright notice in the Description page of Project Settings.


#include "MassEQSSpawnDataGenerator.h"
#include "MassSpawnerTypes.h"
#include "UnitInitializerProcessor.h"
#include "VisualLogger/VisualLogger.h"
#include "MassGameplaySettings.h"

UMassEQSSpawnDataGenerator::UMassEQSSpawnDataGenerator()
{
	EQSRequest.RunMode = EEnvQueryRunMode::AllMatching;
}

void UMassEQSSpawnDataGenerator::Generate(UObject& QueryOwner, TConstArrayView<FMassSpawnedEntityType> EntityTypes, int32 Count, FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const
{
	if (Count <= 0)
	{
		FinishedGeneratingSpawnPointsDelegate.Execute(TArray<FMassEntitySpawnDataGeneratorResult>());
		return;
	}

	// Need to copy the request as it is called inside a CDO and CDO states cannot be changed.
	FEQSParametrizedQueryExecutionRequest EQSRequestInstanced = EQSRequest;
	if (EQSRequestInstanced.IsValid() == false)
	{
		EQSRequestInstanced.InitForOwnerAndBlackboard(QueryOwner, /*BBAsset=*/nullptr);
		if (!ensureMsgf(EQSRequestInstanced.IsValid(), TEXT("Query request initialization can fail due to missing parameters. See the runtime log for details")))
		{
			return;
		}
	}

	// Build array of entity types to spawn.
	// @todo: I dont like that this get's passed by value to OnEQSQueryFinished, but seemed like the cleanest solution.
	TArray<FMassEntitySpawnDataGeneratorResult> Results;
	BuildResultsFromEntityTypes(Count, EntityTypes, Results);

	FQueryFinishedSignature Delegate = FQueryFinishedSignature::CreateUObject(this, &UMassEQSSpawnDataGenerator::OnEQSQueryFinished, Results, FinishedGeneratingSpawnPointsDelegate);
	EQSRequestInstanced.Execute(QueryOwner, /*BlackboardComponent=*/nullptr, Delegate);
}

void UMassEQSSpawnDataGenerator::OnEQSQueryFinished(TSharedPtr<FEnvQueryResult> EQSResult, TArray<FMassEntitySpawnDataGeneratorResult> Results, FFinishedGeneratingSpawnDataSignature FinishedGeneratingSpawnPointsDelegate) const
{
	if (EQSResult.IsValid() == false || EQSResult->IsSuccessful() == false)
	{
		// Return empty result.
		Results.Reset();
		FinishedGeneratingSpawnPointsDelegate.Execute(Results);
		return;
	}

	TArray<FVector> Locations;
	EQSResult->GetAllAsLocations(Locations);

	// Randomize them
	FRandomStream RandomStream(GetRandomSelectionSeed());
	for (int32 I = 0; I < Locations.Num(); ++I)
	{
		const int32 J = RandomStream.RandHelper(Locations.Num());
		Locations.Swap(I, J);
	}

	const int32 LocationCount = Locations.Num();
	int32 LocationIndex = 0;

	// Distribute points amongst the entities to spawn.
	for (FMassEntitySpawnDataGeneratorResult& Result : Results)
	{
		Result.SpawnDataProcessor = UUnitInitializerProcessor::StaticClass();
		Result.SpawnData.InitializeAs<FMassEntitySpawnData>();
		FMassEntitySpawnData& SpawnData = Result.SpawnData.GetMutable<FMassEntitySpawnData>();

		//Transforms
		SpawnData.Transforms.Reserve(Result.NumEntities);
		for (int i = 0; i < Result.NumEntities; i++)
		{
			FTransform& Transform = SpawnData.Transforms.AddDefaulted_GetRef();
			Transform.SetLocation(Locations[LocationIndex % LocationCount]);
			LocationIndex++;
		}

		//ArmyId
		SpawnData.SpawnVars = SpawnVars;
	}

	FinishedGeneratingSpawnPointsDelegate.Execute(Results);
}

void UMassEQSSpawnDataGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	static const FName EQSRequestName = GET_MEMBER_NAME_CHECKED(UMassEQSSpawnDataGenerator, EQSRequest);

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty && PropertyChangedEvent.MemberProperty->GetFName() == EQSRequestName)
	{
		EQSRequest.PostEditChangeProperty(*this, PropertyChangedEvent);
	}
}
