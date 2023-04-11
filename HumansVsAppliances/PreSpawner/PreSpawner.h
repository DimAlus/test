// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HumansVsAppliances/Core/ArenaPlayerController.h"
#include "GameFramework/Actor.h"
#include "PreSpawner.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnActorDelegateSignature, APreSpawner*, actorClass, FVector, locaion);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnActorDelegateSignature, TSubclassOf<AActor>, actorClass, FVector, locaion);

UCLASS()
class HUMANSVSAPPLIANCES_API APreSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APreSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float time = 10;
	bool inited = false;
	TSubclassOf<AActor> ActorClass;
	AArenaPlayerController* MyController;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Init(AArenaPlayerController* controller, TSubclassOf<AActor> actorClass, float timer);

	FOnSpawnActorDelegateSignature OnSpawnActorDelegate;
	/*DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnSpawnActorDelegateSignature, APreSpawner*, spawner, FVector, locaion);
	void OnSpawnActorDelegate(APreSpawner* spawner, FVector locaion);*/
	// Delegate signature

	// Function signature
	/*UPROPERTY(BlueprintAssignable)
	FOnSpawnActorDelegateSignature OnSpawnActorDelegate;*///(APreSpawner* actorClass, FVector locaion);
};
