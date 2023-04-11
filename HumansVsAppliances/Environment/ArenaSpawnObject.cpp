// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaSpawnObject.h"

#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AArenaSpawnObject::AArenaSpawnObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ArenaSpawnObjectSceneComponent"));
}

// Called when the game starts or when spawned
void AArenaSpawnObject::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AArenaSpawnObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
