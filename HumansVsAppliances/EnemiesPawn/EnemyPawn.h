// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "HumansVsAppliances/Interfaces/Score.h"
#include "HumansVsAppliances/Effects/Effect.h"
#include "HumansVsAppliances/Effects/EffectUser.h"
#include "HumansVsAppliances/Effects/EffectInfluenced.h"
#include "HumansVsAppliances/FuncLib/ArenaCommonTypes.h"
#include "HumansVsAppliances/Interfaces/ArenaCharacter.h"
#include "EnemyPawn.generated.h"

UCLASS()
class HUMANSVSAPPLIANCES_API AEnemyPawn : public AArenaCharacter, public IEffectUser, public IEffectInfluenced
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile ")
	FProjectileSetting ProjectileSetting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EnemyType)
	EEnemyType CutEnemyType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attack)
	float CurrentDamage = 0.5f;

	UPROPERTY()
	APlayerController* PlController = nullptr;
	
	UPROPERTY()
	APawn* PlChar = nullptr;

	UPROPERTY()
	AArenaWeapon* CurrentWeapon = nullptr;
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tower)
	bool IsInTowerArea = false;

	virtual void StartTakeDamageAnimation() override;
	void ReturnLastSpeed();

	float LastSpeed = 0.0f;
	FTimerHandle TimerHandle_PlayingDamage;
	

protected:

	/*FVector DifferenceVec = FVector::ZeroVector;*/
	FVector NewDirection  = FVector::ZeroVector;
	FRotator NewRotation  = FRotator::ZeroRotator;

	float CurrentSpeed{};
	float CurrentDeltaTime;
	float PastShootingTime;

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Destroyed() override;

	UFUNCTION(BlueprintCallable, Category = Perf)
	void AttackImpl();

	void NotifyToController();

	UFUNCTION(BlueprintCallable)
	void SetParams(int HealthInPercent, int MaxSpeed);



	virtual AActor* GetJumpingWeapon() override;
	virtual float GetJumpingDamageRadius() override;

protected:
	
	//void MoveToDirection(float DeltaTime);

	//void MoveFaceToPlayer();
	
	virtual void CharacterDead();

public:
	/**  IEffectUser  */

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

	virtual void StartEffectLoading_Implementation() override;


	/**  IEffectInfluenced  */
	virtual void AddAllEffects_Implementation(TArray<UEffect*>& effects, AActor* other, float radius) override;
	virtual void AddEffect_Implementation(UEffect* effect, AActor* other, float radius) override;
	virtual void AddEffectComponent_Implementation(UEffectComponent* component) override;


	virtual void ChangeSpeed_Implementation(float speed) override;
	virtual void ChangeHealth_Implementation(float health, AActor* other) override;
	virtual void ChangeMaxHealth_Implementation(float health, AActor* other) override;
	virtual void ChangeProtection_Implementation(float RateProtect) override;

	virtual void AddWeapon_Implementation(const FString& name) override;
	virtual void AddProjectile_Implementation(int count) override;
	virtual void AddMagazineSize_Implementation(int count) override;

	int countParalisys = 0;
	virtual void SetParalisys_Implementation(bool isParalisys) override;
	virtual bool IsParalisys_Implementation() override;

	virtual void Repuls_Implementation(float power, FVector dir) override;
};
