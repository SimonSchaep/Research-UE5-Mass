// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntitySpawnDataGeneratorBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "MassEQSSpawnDataGenerator.generated.h"

//Struct that holds all custom spawn data
USTRUCT(BlueprintType)
struct FMassEntitySpawnVars
{
	GENERATED_BODY()

	FMassEntitySpawnVars() {}
	
	UPROPERTY(EditAnyWhere)
	int ArmyId = 0;

	UPROPERTY(EditAnyWhere)
	bool bShouldOverrideSpawnRotation = false;

	UPROPERTY(EditAnyWhere)
	FQuat SpawnRotationOverride{};
};

//Based on FMassTransformsSpawnData
USTRUCT()
struct FMassEntitySpawnData
{
	GENERATED_BODY()

	FMassEntitySpawnData()
	: bRandomize(true)
	{
	}

	// declaring the type used to be able to statically test it against other types 
	using FTransformsContainerType = TArray<FTransform>;
	FTransformsContainerType Transforms;

	// When true, Transforms will be assigned to entities as if pre-shuffled. 
	// If Transforms >= Entities, this provides the best chance for a good spread of entity transforms 
	// and transforms choices will be guaranteed unique. If Transforms < Entities, the same 
	// transform will be used for multiple entities and a warning logged.
	//
	// When false, Transforms will be assigned in order to spawned entities.
	bool bRandomize : 1;

	FMassEntitySpawnVars SpawnVars;
};

//Based on UMassEntityEQSSpawnPointsGenerator
UCLASS(BlueprintType, meta = (DisplayName = "EQS SpawnData Generator"))
class BATTLESIMULATORMASS_API UMassEQSSpawnDataGenerator : public UMassEntitySpawnDataGeneratorBase
{
	GENERATED_BODY()
	
public:
	UMassEQSSpawnDataGenerator();

	virtual void Generate(UObject& QueryOwner, TConstArrayView<FMassSpawnedEntityType> EntityTypes, int32 Count, FFinishedGeneratingSpawnDataSignature& FinishedGeneratingSpawnPointsDelegate) const override;


protected:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void OnEQSQueryFinished(TSharedPtr<FEnvQueryResult> EQSResult, TArray<FMassEntitySpawnDataGeneratorResult> Results, FFinishedGeneratingSpawnDataSignature FinishedGeneratingSpawnPointsDelegate) const;

	UPROPERTY(Category = "Query", EditAnywhere)
		FEQSParametrizedQueryExecutionRequest EQSRequest;

	UPROPERTY(Category = "Query", EditAnywhere)
		FMassEntitySpawnVars SpawnVars;
};
