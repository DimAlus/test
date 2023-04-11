// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ArenaPawn.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIArenaPawn : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HUMANSVSAPPLIANCES_API IIArenaPawn
{
	GENERATED_BODY()

public:
	bool bIsAttack = false;
	bool bIsDefend = false;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	virtual void Attack() = 0;
	virtual void StartDefend() = 0;
	virtual void StopDefend() = 0;
	virtual void RecoveringDefend() = 0;
};
