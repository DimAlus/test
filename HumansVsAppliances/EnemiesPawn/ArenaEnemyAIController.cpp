// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaEnemyAIController.h"
#include "EnemyPawn.h"

void AArenaEnemyAIController::RetargetToPlayerPawn_BP_Implementation(ACharacter* MyPawn)
{
	
}

void AArenaEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AArenaEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	CurSide = static_cast<ESides>(FMath::RandRange(0, 3));
	CurPawn = Cast<AEnemyPawn>(InPawn);
	
}

void AArenaEnemyAIController::StartMeleeLogic()
{
	FVector CurDirection;
	if (bIsStartAttack)
	{
		CurDirection = CurPawn->PlChar->GetActorLocation();
		CurPawn->AttackImpl();
	}
	else
	{
		
		switch (CurSide) {
		case ESides::Front:
			CurDirection = CurPawn->PlChar->GetActorForwardVector() * DirFactor + CurPawn->PlChar->GetActorLocation();
			break;
		case ESides::Back:
			CurDirection = CurPawn->PlChar->GetActorForwardVector() * -DirFactor + CurPawn->PlChar->GetActorLocation();
			break;
		case ESides::Left:
			CurDirection = CurPawn->PlChar->GetActorRightVector() * -DirFactor + CurPawn->PlChar->GetActorLocation();
			break;
		case ESides::Right:
			CurDirection = CurPawn->PlChar->GetActorRightVector() * DirFactor + CurPawn->PlChar->GetActorLocation();
			break;
		default:
			break;
		}
		
			
		if (FVector::Dist(CurPawn->GetActorLocation(), CurDirection) < 100)
		{
			bIsStartAttack = true;
		}
		
	}
	MoveToLocation(CurDirection);

}

void AArenaEnemyAIController::StartDistanceLogic()
{
}

void AArenaEnemyAIController::Tick(float DeltaTime)
{
	CurDeltaTime += DeltaTime;
	if (CurDeltaTime-PastCurDeltaTime >= TickToDealLogic)
	{
		PastCurDeltaTime = CurDeltaTime;
		if (CurPawn && CurPawn->PlController && CurPawn->PlChar)
		{

			if (CurPawn->CutEnemyType == EEnemyType::Router || CurPawn->CutEnemyType == EEnemyType::RobotVacuum)
				StartMeleeLogic();
			else if (CurPawn->CutEnemyType == EEnemyType::Washer)
				StartDistanceLogic();
		}
	}
	
	Super::Tick(DeltaTime);
}
