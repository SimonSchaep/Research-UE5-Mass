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

	UFUNCTION(BlueprintCallable)
	void RestartGame();

	class UUnitManager* GetUnitManager()const;

private:
	UPROPERTY(EditAnywhere)
		class UUnitManager* UnitManager;
};
