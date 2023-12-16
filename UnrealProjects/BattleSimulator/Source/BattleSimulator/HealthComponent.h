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
	// Sets default values for this component's properties
	UHealthComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeDamage);
	UPROPERTY(BlueprintAssignable)
		FOnTakeDamage OnTakeDamage;

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
