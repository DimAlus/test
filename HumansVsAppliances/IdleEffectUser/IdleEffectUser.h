// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HumansVsAppliances/FuncLib/ArenaCommonTypes.h"
#include "HumansVsAppliances/Effects/Effect.h"
#include "HumansVsAppliances/Effects/EffectUser.h"
#include "HumansVsAppliances/Effects/EffectInfluenced.h"
#include "IdleEffectUser.generated.h"




/////////////////////////////
////////	Drops
////////////////////////////


UENUM(BlueprintType)
enum class EDropType : uint8
{
	COINS			UMETA(DisplayName = COINS)
	, RED			UMETA(DisplayName = RED)
	, ORANGE		UMETA(DisplayName = ORANGE)
	, GREEN			UMETA(DisplayName = GREEN)
};

USTRUCT(BlueprintType)
struct FEnemyDropData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> DropTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDropType Type;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float chance = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString comment;
};
USTRUCT(BlueprintType)
struct FEnemyDropDataArray
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyDropData> datas;
};



USTRUCT(BlueprintType)
struct FEnemyDropChoiser
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AIdleEffectUser> DropClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Range;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FEnemyDropData> OneFrom;

	UPROPERTY()
		float sumchances = -1;
};

USTRUCT(BlueprintType)
struct FEnemyDrop
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AIdleEffectUser> DropClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float chance;
	UPROPERTY()
		float limit;
};



UCLASS()
class HUMANSVSAPPLIANCES_API AIdleEffectUser : public AActor,
	public IEffectUser,
	public IEffectInfluenced
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIdleEffectUser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	FTimerHandle TimerHandle_Shooting;
	FTimerHandle TimerHandle_Death;
	int NumOfShots = 0;

	bool loaded = false;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnOtherActorBeginOverlap(AActor* OtherActor);
	

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIdleEffectUserInfo CurrentInfo;

	void Dead();
	void StartFire();
	void Fire();

	UFUNCTION(BlueprintCallable)
	virtual void Init();
	UFUNCTION(BlueprintCallable)
	virtual void InitWithEffects(TArray<FEnemyDropData>& AddEffects);
	UFUNCTION(BlueprintCallable)
	TArray<FEnemyDropData> GetEffectsToPlayer();
	/**  IEffectUser  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	TArray<FEnemyDropData> EffectsToPlayer;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		TArray<UEffect*> EffectsToProjectiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
		TArray<UEffect*> EffectsToBuffEffects;
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

	virtual void SetParalisys_Implementation(bool isParalisys) override;
	virtual bool IsParalisys_Implementation() override;
};
