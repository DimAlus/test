// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HumansVsAppliances/PlayerPawn/ArenaPlayerPawn.h"
#include "ArenaEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class HUMANSVSAPPLIANCES_API AArenaEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void RetargetToPlayerPawn_BP(ACharacter* MyPawn);

	UPROPERTY()
	AEnemyPawn* CurPawn = nullptr;

	
	bool bIsStartAttack;
	ESides CurSide;
	float DirFactor = 500;
	float TickToDealLogic = 0.2;
	float CurDeltaTime;
	float PastCurDeltaTime;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	void StartMeleeLogic();
	void StartDistanceLogic();

public:
	virtual void Tick(float DeltaTime) override;
};
