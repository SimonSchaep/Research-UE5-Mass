// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionActor.h"
#include "AnimationSharingManager.h"

//UAnimationSharingManager::UnregisterActor is not accessible from bp, so we add a helper function here
void AMinionActor::UnregisterFromAnimSharing()
{
	UAnimationSharingManager::GetManagerForWorld(GetWorld())->UnregisterActor(this);
}
