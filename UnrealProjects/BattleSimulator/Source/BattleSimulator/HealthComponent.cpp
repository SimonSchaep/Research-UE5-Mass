// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "UnitManager.h"
#include "BattleSim.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	ABattleSim* GameMode = Cast<ABattleSim>(GetWorld()->GetAuthGameMode());
	UnitManager = GameMode->GetUnitManager();	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::DoDamage(float Amount)
{
	CurrentHealth -= Amount;
	OnTakeDamage.Broadcast();
	if (CurrentHealth <= 0)
	{
		UnitManager->UnRegisterUnit(GetOwner());
		GetOwner()->Destroy();
	}
}

