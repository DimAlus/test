// Fill out your copyright notice in the Description page of Project Settings.


#include "PreSpawner.h"

// Sets default values
APreSpawner::APreSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APreSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APreSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if ((time -= DeltaTime) < 0) {
		if (inited)
			OnSpawnActorDelegate.Broadcast(ActorClass, GetActorLocation());
		Destroy();
	}
	
}
void APreSpawner::Init_Implementation(AArenaPlayerController* controller, TSubclassOf<AActor> actorClass, float timer) {
	this->time = timer;
	this->ActorClass = actorClass;
	inited = true;
}

