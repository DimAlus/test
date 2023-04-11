// Copyright Epic Games, Inc. All Rights Reserved.


#include "ArenaGameModeBase.h"
#include "ArenaPlayerController.h"

/** set spawn platform */
void AArenaGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/*if (AArenaPlayerController* MyController = Cast<AArenaPlayerController>(NewPlayer))
	{
		MyController->SetSpawnPlatform();
	}*/
}


/** respawn character and possess in current controller */
bool AArenaGameModeBase::RespawnCharacter(AArenaPlayerController* CurrentController) const
{
	if (CurrentController)
	{
		const FVector MySpawnLocation = CurrentController->SurfacePlatform->GetActorLocation() + FVector(200, 200,400);
		const FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.bDeferConstruction = false;

		AArenaPlayerPawn* NewCharacter = Cast<AArenaPlayerPawn>(GetWorld()->SpawnActor(RespawningCharacter,
																						&MySpawnLocation,
																						&SpawnRotation,
																						SpawnParams));
	
		if (NewCharacter)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Character born!"));
			CurrentController->Possess(NewCharacter);
			return true;
		}
		return false;
	}
	return false;
}
