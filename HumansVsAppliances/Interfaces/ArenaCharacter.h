// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ArenaCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

UCLASS()
class HUMANSVSAPPLIANCES_API AArenaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArenaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class UArenaHealthActorComponent* HealthComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* AnimTakeDamage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	UAnimMontage* AnimDead = nullptr;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category="Movement")
	FOnDead OnDead;

	bool bIsJumping = false;
	bool bReadyJumpingDamage = false;
	bool bIsAlive = true;
	float MaxJumpDamageDistance = 300.0f;
	float MinJumpDamageDistance = 200.0f;


	FTimerHandle TimerHandle_PlayingDeathAnim;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual AActor* GetJumpingWeapon();
	virtual float GetJumpingDamageRadius();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void CharacterDeadAnim();

	virtual void CharacterDead();

	virtual void StartTakeDamageAnimation();
	
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent)
	void PlayDamageSound_BP();
	
};

