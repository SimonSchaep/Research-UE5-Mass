// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinionActor.generated.h"

UCLASS()
class BATTLESIMULATORMASS_API AMinionActor : public AActor
{
	GENERATED_BODY()
	
private:
	//UAnimationSharingManager::UnregisterActor is not accessible from bp, so we add a helper function here
	UFUNCTION(BlueprintCallable)
	void UnregisterFromAnimSharing();

};
