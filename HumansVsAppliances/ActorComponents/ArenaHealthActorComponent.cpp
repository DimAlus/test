// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaHealthActorComponent.h"

// Sets default values for this component's properties
UArenaHealthActorComponent::UArenaHealthActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UArenaHealthActorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UArenaHealthActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/** get current health */
float UArenaHealthActorComponent::GetCurrentHealth()
{
	return CurrentHealth;
}

/** change value of current health */
void UArenaHealthActorComponent::ChangeHealthValue(float ChangeValue)
{
	CurrentHealth += ChangeValue * RateDamage;
	OnHealthChange.Broadcast(CurrentHealth/MaxHealth);
	
	if (CurrentHealth > MaxHealth) CurrentHealth = MaxHealth;
	else if (CurrentHealth <= 0.0f) OnHealthEmpty.Broadcast();
}