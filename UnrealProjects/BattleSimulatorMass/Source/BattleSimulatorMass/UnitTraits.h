// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MassEntityTraitBase.h"
#include "UnitFragments.h"
#include "UnitTraits.generated.h"


// ArmyId
UCLASS()
class BATTLESIMULATORMASS_API UArmyIdTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()
	
protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;
};

// Attack
UCLASS()
class BATTLESIMULATORMASS_API UAttackTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

private:
	UPROPERTY(EditAnywhere)
		FUnitAttackParameters AttackParameters;
};

// DeathHandling
UCLASS()
class BATTLESIMULATORMASS_API UDeathHandlingTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

private:
	UPROPERTY(EditAnywhere)
		FUnitDeathParameters DeathParameters;
};

// Health
UCLASS()
class BATTLESIMULATORMASS_API UHealthTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

private:

	UPROPERTY(EditAnywhere)
		float Health = 0;
};

// Move
UCLASS()
class BATTLESIMULATORMASS_API UMoveTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

private:
	UPROPERTY(EditAnywhere)
		FUnitMoveParameters MoveParameters;
};

// TargetAcquisition
UCLASS()
class BATTLESIMULATORMASS_API UTargetAcquisitionTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

};

// Animation
UCLASS()
class BATTLESIMULATORMASS_API UUnitAnimationTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;
};

// TransformSync
UCLASS()
class BATTLESIMULATORMASS_API UUnitTransformSyncTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World) const override;
};

// Visualization
UCLASS()
class BATTLESIMULATORMASS_API UUnitVisualizationTrait : public UMassEntityTraitBase
{
	GENERATED_BODY()

protected:
	virtual void BuildTemplate(FMassEntityTemplateBuildContext& BuildContext, const UWorld& World)const override;

private:
	UPROPERTY(EditAnywhere)
		FUnitVisualizationParameters VisualizationParameters;
};