// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArenaHealthActorComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChange, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthEmpty);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HUMANSVSAPPLIANCES_API UArenaHealthActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArenaHealthActorComponent();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category="Movement")
	FOnHealthChange OnHealthChange;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category="Movement")
	FOnHealthEmpty OnHealthEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float RateDamage = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;

	float CurrentHealth = 100.0f;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Health")
	virtual void ChangeHealthValue(float ChangeValue);

	UFUNCTION(BlueprintCallable, Category="Health")
	float GetCurrentHealth();
		
};
