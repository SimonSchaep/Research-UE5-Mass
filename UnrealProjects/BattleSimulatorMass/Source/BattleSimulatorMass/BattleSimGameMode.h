// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleSimGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLESIMULATORMASS_API ABattleSimGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleSimGameMode();

	virtual void StartPlay()override;

	UFUNCTION(BlueprintCallable)
		void StartSpawning();

	UFUNCTION(BlueprintCallable)
		void Restart();

private:
	class TArray<class AMassSpawner*> ArmySpawners;
};
