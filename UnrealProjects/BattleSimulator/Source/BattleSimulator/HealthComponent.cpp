// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "UnitManager.h"
#include "BattleSimGameMode.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	ABattleSimGameMode* GameMode = Cast<ABattleSimGameMode>(GetWorld()->GetAuthGameMode());
	UnitManager = GameMode->GetUnitManager();	
}

void UHealthComponent::DoDamage(float Amount)
{
	CurrentHealth -= Amount;
	if (CurrentHealth <= 0)
	{
		UnitManager->UnRegisterUnit(GetOwner());
		OnDeath.Broadcast();
	}
}

bool UHealthComponent::HasDied() const
{
	return CurrentHealth <= 0;
}

