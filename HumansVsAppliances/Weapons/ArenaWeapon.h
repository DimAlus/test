// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HumansVsAppliances/FuncLib/ArenaCommonTypes.h"
#include "HumansVsAppliances/Effects/Effect.h"
#include "HumansVsAppliances/Effects/EffectUser.h"
#include "HumansVsAppliances/Effects/EffectInfluenced.h"
#include "HumansVsAppliances/EnemiesPawn/EnemyPawn.h"
#include "ArenaWeapon.generated.h"

UCLASS()
class HUMANSVSAPPLIANCES_API AArenaWeapon : public AActor, 
											public IEffectUser, 
											public IEffectInfluenced
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	AActor* MyOwner;
	
	EOwnerType OwnerType;
	
	

private:
	void Fire();

	APlayerController* CurPlayerController = nullptr;
	
public:	
	// Sets default values for this actor's properties
	AArenaWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USceneComponent* SceneComponent = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	class UStaticMeshComponent* StaticMeshWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class UArrowComponent* ShootLocation = nullptr;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	bool bIsJumping{false};*/


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	FWeaponInfo CurrentWeaponInfo;
	
	

	UPROPERTY()
	AArenaCharacter* CurCharacter = nullptr;
	
	FVector WorldLocation = FVector::ZeroVector;
	FVector WorldDirection = FVector::ZeroVector;
	FVector EndLocProjectile = FVector::ZeroVector;

	FTimerHandle TimerHandle_Shooting;
	FName NameWeapon;
	int NumOfShots = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	void SpawnProjectile(FVector CurrentVector);

	void CalculateEndLocationOfProjectile();

	void CircleRepulsion();

	void JumpingMech();

	void ActivateJumpDamage();

	UFUNCTION(BlueprintNativeEvent)
	void SpawnEffect_BP(UParticleSystem* CurFX);

	FTimerHandle TimerHandle_ActivateJumpDamage;
	
	float JumpingTime = 1.0f;

	void LineRepulsion();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Shooting();

	void SetCurPlayerController(APlayerController* NewController);

	UFUNCTION(BlueprintNativeEvent)
	void PlayFireSound_BP(USoundBase* SoundFireWeapon);


	/**  IEffectUser  */
	bool bVasStarted = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> EffectsToProjectiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> EffectsToBuffEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> OtherEffects;

	int EffectIndex = 1;

	UFUNCTION(BlueprintCallable)
	void SetMyOwner(AActor* myowne);
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

	virtual void SetParalisys_Implementation(bool isParalisys) override;
	virtual bool IsParalisys_Implementation() override;

};
