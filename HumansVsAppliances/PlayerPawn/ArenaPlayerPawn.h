// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "HumansVsAppliances/FuncLib/ArenaCommonTypes.h"
#include "HumansVsAppliances/Interfaces/ArenaPawn.h"
#include "HumansVsAppliances/Effects/Effect.h"
#include "HumansVsAppliances/Effects/EffectUser.h"
#include "HumansVsAppliances/Effects/EffectInfluenced.h"
#include "HumansVsAppliances/Interfaces/ArenaCharacter.h"
#include "ArenaPlayerPawn.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeStateRocket , bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeStateLaser , bool, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeStateJump , bool, NewValue);


UCLASS()
class HUMANSVSAPPLIANCES_API AArenaPlayerPawn : public AArenaCharacter, public IEffectUser, public IEffectInfluenced
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AArenaPlayerPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USkeletalMeshComponent* SkeletalMeshPlayer = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rocket)
	FProjectileSetting RocketProjectileSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rocket)
	int ShellsInShooting = 1; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	int32 MaxAngleBetweenMovementAndCursor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Movement)
	FPawnSpeed CurrentSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	bool bIsSprint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	AArenaWeapon* CurrentWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	bool bIsAlive{true};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rotation)
	bool IsCanRotate{true};
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	bool bIsJumping{false};*/


	
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = Activate)
	FOnChangeStateRocket OnChangeStateRocket;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = Activate)
	FOnChangeStateLaser OnChangeStateLaser;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = Activate)
	FOnChangeStateJump OnChangeStateJump;

	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Activate)
	FTimerHandle TimerHandle_ActivateRocket;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Activate)
	FTimerHandle TimerHandle_ActivateLaser;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Activate)
	FTimerHandle TimerHandle_ActivateJump;

	
	bool IsActiveRocket{true};
	bool IsActiveLaser{true};
	bool IsActiveJump{true};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Activate)
	float TimeToActiveRocket = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Activate)
	float TimeToActiveLaser = 3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Activate)
	float TimeToActiveJump = 3;

	void LockRocket();
	void ActivateRocket();

	void LockLaser();
	void ActivateLaser();

	void LockJump();
	void ActivateJump();

protected:

	// Movement
	float AxisX = 0.0f;
	float AxisY = 0.0f;
	float YawToCursor = 0.0f;

	EProjectileType UsingProjectile{EProjectileType::Simple};
	
	// Skill Mechanics

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Jump)
	float PowerToJump = 700;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Repulsion)
	float PowerRepulsion = 700;

	//FTimerHandle TimerHandle_ActivateJumpDamage;
	
	//float JumpingTime = 1.0f;

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void PossessedBy(AController* NewController) override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
							 AActor* DamageCauser) override;

	/*UFUNCTION()    
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/


	UFUNCTION()
	void SpawnWeaponsByNames(UInputComponent* PlayerInputComponent, TArray<FString> wpns);

	UPROPERTY()
	TArray<FString> MyWeapons;

protected:

	// Movement
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
	
	void CalculateNewCharacterRotation();
	void MovementTick();

	// Attack

	UFUNCTION()
	void UseWeapon(FName NameWeapon, bool IsOnce = false);
	
	UFUNCTION()
	void UsePistol();

	UFUNCTION()
	void UseRifle();
	
	UFUNCTION()
	void UseMachineGun();

	UFUNCTION()
	void BreadLauncher();

	UFUNCTION()
	void BurntBreadLauncher();

	// Mechanics

	UFUNCTION()
	void Jumping();

	/*UFUNCTION()
	void ActivateJumpDamage();*/

	UFUNCTION()
	void LineRepulsion();

	UFUNCTION()
	void CircleRepulsion();

	UFUNCTION()
	void RocketLaunch();

	UFUNCTION()
	virtual AActor* GetJumpingWeapon() override;
	virtual float GetJumpingDamageRadius() override;

	UPROPERTY()
	AArenaWeapon* jumpingWeapon = nullptr;

public:

	/**  IEffectUser  */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> WeaponEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> OtherEffects;
	int EffectIndex = 1;

	virtual AActor* GetMyOwner_Implementation() override;

	virtual void LoadEffect_Implementation(UEffectsMeta* effectMeta) override;
	virtual void LoadAllEffects_Implementation(TArray<UEffectsMeta*>& effectsMeta) override;
	virtual int ApplyEffect_Implementation(UEffect* effect, int toIndex) override;
	virtual void ApplyAllEffects_Implementation(TArray<UEffect*>& effects, int toIndex) override;
	virtual void CancelEffectByIndex_Implementation(int index) override;
	virtual TArray<int> AddEffectsToWeapon_Implementation(const TArray<UEffect*>& wepeffects) override;
	virtual TArray<UEffect*> GetEffectsToWeapon_Implementation() override;

	virtual TArray<UEffect*> GetAllEffectsOnDistantion_Implementation(float distantion, AActor* who) override;
	virtual TArray<UEffect*> GetAllEffectsOnPoint_Implementation(AActor* who) override;
	virtual TArray<UEffect*> GetAllEffects_Implementation(AActor* who) override;
	virtual float MaxEffectRadius_Implementation() override;
	virtual bool IsPointDamage_Implementation() override;

	virtual void AddCoins_Implementation(int count) override;

	virtual void StartEffectLoading_Implementation() override;



	/**  IEffectInfluenced  */
	virtual void AddAllEffects_Implementation(TArray<UEffect*>& effects, AActor* other, float radius) override;
	virtual void AddEffectFromMeta_Implementation(UEffectsMeta* effect, AActor* other, float radius) override;
	virtual void AddEffect_Implementation(UEffect* effect, AActor* other, float radius) override;
	virtual void AddEffectComponent_Implementation(UEffectComponent* component) override;


	virtual void ChangeSpeed_Implementation(float speed) override;
	virtual void ChangeHealth_Implementation(float health, AActor* other) override;
	virtual void ChangeMaxHealth_Implementation(float health, AActor* other) override;
	virtual void ChangeProtection_Implementation(float RateProtect) override;

	UPROPERTY()
	UInputComponent* MyPlayerInputComponent;

	virtual void AddWeapon_Implementation(const FString& name) override;
	virtual void AddProjectile_Implementation(int count) override;
	virtual void AddMagazineSize_Implementation(int count) override;

	int countParalisys = 0;
	virtual void SetParalisys_Implementation(bool isParalisys) override;
	virtual bool IsParalisys_Implementation() override;

	virtual void Repuls_Implementation(float power, FVector dir) override;

	//virtual void Destroyed() override;
};
