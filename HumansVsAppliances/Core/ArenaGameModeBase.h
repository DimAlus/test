// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ArenaPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class HUMANSVSAPPLIANCES_API AArenaGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AArenaPlayerPawn> RespawningCharacter = nullptr;
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	bool RespawnCharacter(AArenaPlayerController* CurrentController) const;
	
private:
};
