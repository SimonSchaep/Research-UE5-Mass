// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLESIMULATOR_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();

	virtual void BeginPlay() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
	UPROPERTY(BlueprintAssignable)
		FOnDeath OnDeath;

	void DoDamage(float Amount);

	UFUNCTION(BlueprintCallable)
	bool HasDied()const;

private:
	class UUnitManager* UnitManager;

	UPROPERTY(EditAnywhere)
		float CurrentHealth = 100;
		
};
