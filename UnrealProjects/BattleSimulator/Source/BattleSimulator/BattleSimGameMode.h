// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleSimGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATOR_API ABattleSimGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABattleSimGameMode();

	virtual void StartPlay()override;

	class UUnitManager* GetUnitManager()const;

	UFUNCTION(BlueprintCallable)
		void StartSpawning();

	UFUNCTION(BlueprintCallable)
		void StartSimulation();

	UFUNCTION(BlueprintCallable)
	void Restart();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartSimulation);
	UPROPERTY(BlueprintAssignable)
		FOnStartSimulation OnStartSimulation;

	UFUNCTION(BlueprintCallable)
		bool HasSpawned()const;

	UFUNCTION(BlueprintCallable)
		bool HasStartedSimulation()const;

protected:
	UPROPERTY(BlueprintReadOnly)
		bool bHasSpawned = false;

	UPROPERTY(BlueprintReadOnly)
		bool bHasStartedSimulation = false;

private:
	UPROPERTY(EditAnywhere)
		class UUnitManager* UnitManager;
};
