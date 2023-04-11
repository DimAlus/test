// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../FuncLib/ArenaCommonTypes.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "HumansVsAppliances/Effects/Effect.h"
#include "HumansVsAppliances/Effects/EffectUser.h"
#include "HumansVsAppliances/Effects/EffectInfluenced.h"
#include "ArenaProjectile.generated.h"

UCLASS()
class HUMANSVSAPPLIANCES_API AArenaProjectile : public AActor, public IEffectUser, public IEffectInfluenced
{
	GENERATED_BODY()
	
public:
	AArenaProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	USphereComponent* CollisionComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	UStaticMeshComponent* ProjectileMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USkeletalMeshComponent* ProjectileSkeletalMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = Components)
	class UParticleSystemComponent* BulletFX = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	FProjectileSetting ProjectileSetting;

	float Damage{};
	
	FTimerHandle TimerHandle_Сlimb;

	float MaxDamageDistance{};
	float MinDamageDistance{};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	AActor* MyOwner;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Function that is called when the projectile hits something.
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// set end direction for projectile
	void MoveToDirection(const FVector& ShootDirection) const;

	UFUNCTION()
	void OutClimb(const FVector& ShootDirection) const;

	void InitProjectile(FProjectileSetting NewProjectileSetting,
						const FVector& ShootDirection,
						float InputDamage,
						float InputMaxDamageDistance,
						float InputMinDamageDistance);

	UFUNCTION(BlueprintCallable)
	void SetMyOwner(AActor* Myowne);

	/**  IEffectUser  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> EffectsToEnemy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> EffectsToBuffEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<UEffect*> OtherEffects;

	UPROPERTY(BlueprintReadWrite)
	bool isPointDamage;

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

	virtual void SetParalisys_Implementation(bool isParalisys) override;
	virtual bool IsParalisys_Implementation() override;
};
