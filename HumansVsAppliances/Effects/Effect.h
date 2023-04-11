// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Effect.generated.h"


UENUM(BlueprintType)
enum class EEffectTypes : uint8 {
	NONE					UMETA(DisplayName = "NONE")
	/**  INFLUENCE  */
	, MOMENTAL_DAMAGE		UMETA(DisplayName = "MOMENTAL_DAMAGE")
	, PERIODIC_DAMAGE		UMETA(DisplayName = "PERIODIC_DAMAGE")
	, HEAL					UMETA(DisplayName = "HEAL")
	, PARALISYS				UMETA(DisplayName = "PARALISYS")
	, REPULSION				UMETA(DisplayName = "REPULSION")
	, ADD_EFFECTS			UMETA(DisplayName = "ADD_EFFECTS")

	/**  UP_EFFECT  */
	, UP_ALL_EFFECTS		UMETA(DisplayName = "UP_ALL_EFFECTS")
	, UP_MOMENTAL_DAMAGE	UMETA(DisplayName = "UP_MOMENTAL_DAMAGE")
	, UP_PERIODIC_DAMAGE	UMETA(DisplayName = "UP_PERIODIC_DAMAGE")
	, UP_HEAL				UMETA(DisplayName = "UP_HEAL")
	, UP_PARALISYS			UMETA(DisplayName = "UP_PARALISYS")

	/**  UP_USER  */
	, MAX_HELTH				UMETA(DisplayName = "MAX_HELTH")
	, SPEED					UMETA(DisplayName = "SPEED")
	, PROTECTION			UMETA(DisplayName = "PROTECTION")
	, ADD_PROJECTILE		UMETA(DisplayName = "ADD_PROJECTILE")
	, ADD_MAGAZINE_SIZE		UMETA(DisplayName = "ADD_MAGAZINE_SIZE")

	, ADD_TO_WEAPON			UMETA(DisplayName = "ADD_TO_WEAPON")
	, WEAPON_PISTOL			UMETA(DisplayName = "WEAPON_PISTOL")
	, WEAPON_RIFLE			UMETA(DisplayName = "WEAPON_RIFLE")
	, WEAPON_MACHINEGUN		UMETA(DisplayName = "WEAPON_MACHINEGUN")
	, WEAPON_LINE_REPULS	UMETA(DisplayName = "WEAPON_LINE_REPULSION")
	, WEAPON_CIRC_REPULS	UMETA(DisplayName = "WEAPON_CIRCLE_REPULSION")
	, WEAPON_BREAD			UMETA(DisplayName = "WEAPON_BREAD")
	, WEAPON_BURNT_BREAD	UMETA(DisplayName = "WEAPON_BURNT_BREAD")
	, WEAPON_ROCKET			UMETA(DisplayName = "WEAPON_ROCKET")
	, WEAPON_JUMPING		UMETA(DisplayName = "JUMPING")
	, WEAPON_MELEE			UMETA(DisplayName = "WEAPON_MELEE")
	, WEAPON_ACID			UMETA(DisplayName = "WEAPON_ACID")

	, ADD_COINS				UMETA(DisplayName = "COINS")
};

UENUM(BlueprintType)
enum class ETepesEffectType : uint8 {
	INFLUENCE		UMETA(DisplayName = "INFLUENCE")
	, UP_EFFECT		UMETA(DisplayName = "UP_EFFECT")
	, UP_USER		UMETA(DisplayName = "UP_USER")
};

UENUM(BlueprintType)
enum class EEffectAim : uint8 {
	ALL				UMETA(DisplayName = "ALL")
	, PLAYER		UMETA(DisplayName = "PLAYER")
	, ENEMY			UMETA(DisplayName = "ENEMY")
};

UCLASS(Blueprintable)
class HUMANSVSAPPLIANCES_API UEffectsMeta : public UObject
{
	GENERATED_BODY()

public:
	void Copy(UEffectsMeta* meta);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectTypes effectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectAim effectAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float power;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radianalPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float period;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEffectsMeta*> linkedEffects;

	//TArray< UEffectsMeta> effs;
};

USTRUCT(BlueprintType)
struct FEffectMetaArray
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEffectsMeta*> metas;
};

USTRUCT(BlueprintType)
struct FSkil
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Tag; // Who use skill

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEffectsMeta*> effects;
};


USTRUCT(BlueprintType)
struct FSkillTreeLevel
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString comment;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString owner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> tags;


};
USTRUCT(BlueprintType)
struct FSkillTreeElem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int index;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int index;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> parents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> prices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString comment;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkillTreeLevel> levels;

};
/**
 *
 */
UCLASS(Blueprintable)
class HUMANSVSAPPLIANCES_API UEffect : public UObject
{
	GENERATED_BODY()

protected:
	float ticks = 0;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectTypes effectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectAim effectAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float power;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float period;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radianalPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float time;

	UPROPERTY(BlueprintReadOnly)
	TArray<EEffectTypes> linkedTypes;
	UPROPERTY(BlueprintReadOnly)
	TArray<UEffect*> linkedEffects;
	UPROPERTY(BlueprintReadOnly)
	TArray<int> linkedIndexes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int index;


	UFUNCTION(BlueprintCallable)
	virtual void Init(UEffectsMeta* meta);
	UFUNCTION(BlueprintCallable)
	virtual void Copy(UEffect* copy);


	UFUNCTION(BlueprintCallable)
	virtual void CallStart(AActor* owner, AActor* other);
	UFUNCTION(BlueprintCallable)
	virtual void CallTick(float deltaTime, AActor* owner, AActor* other);
	UFUNCTION(BlueprintCallable)
	virtual void CallEnd(AActor* owner, AActor* other);
	UFUNCTION(BlueprintCallable)
	virtual void CallCancel(AActor* owner, AActor* other);
	UFUNCTION(BlueprintCallable)
	virtual void CallBuffEffect(UEffect* effect, AActor* owner);
	UFUNCTION(BlueprintCallable)
	virtual void CallDeBuffEffect(UEffect* effect, AActor* owner);

	UFUNCTION(BlueprintCallable)
	virtual float GetPowerByLength(float rad);

	ETepesEffectType GetTypeOfType();
	static ETepesEffectType GetTypeOfType(EEffectTypes tp);

protected:
	void (*OnStart)(UEffect* CurEffect, AActor* owner, AActor* other);
	void (*OnTick)(UEffect* CurEffect, float deltaTime, AActor* owner, AActor* other);
	void (*OnEnd)(UEffect* CurEffect, AActor* owner, AActor* other);
	void (*OnCancel)(UEffect* CurEffect, AActor* owner, AActor* other);
	void (*OnBuffEffect)(UEffect* CurEffect, UEffect*, AActor*);
	void (*OnDeBuffEffect)(UEffect* CurEffect, UEffect*, AActor*);



	static void (* GetOnStartFunc(UEffectsMeta* meta))(UEffect* CurEffect, AActor* owner, AActor* other);
	static void (* GetOnTickFunc(UEffectsMeta* meta))(UEffect* CurEffect, float deltaTime, AActor* owner, AActor* other);
	static void (* GetOnEndFunc(UEffectsMeta* meta))(UEffect* CurEffect, AActor* owner, AActor* other);
	static void (* GetOnCancelFunc(UEffectsMeta* meta))(UEffect* CurEffect, AActor* owner, AActor* other);
	static void (* GetBuffEffectFunc(UEffectsMeta* meta))(UEffect* CurEffect, UEffect*, AActor*);
	static void (* GetDeBuffEffectFunc(UEffectsMeta* meta))(UEffect* CurEffect, UEffect*, AActor*);

	static TArray<EEffectTypes> GetLinkedEffects(UEffectsMeta* meta);


private:
	static void BuffEffect(UEffect* CurEffect, UEffect* effect, AActor* owner);
	static void DeBuffEffect(UEffect* CurEffect, UEffect* effect, AActor* owner);

	static void AddEffectToUser(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveEffectFromUser(UEffect* CurEffect, AActor* owner, AActor* other);

	static void AddSpeed(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveSpeed(UEffect* CurEffect, AActor* owner, AActor* other);

	static void AddHealth(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveHealth(UEffect* CurEffect, AActor* owner=nullptr, AActor* other=nullptr);
	static void PeriodicalAddHealth(UEffect* CurEffect, float deltaTime, AActor* owner, AActor* other);
	static void PeriodicalRemoveHealth(UEffect* CurEffect, float deltaTime, AActor* owner, AActor* other);

	static void AddMaxHealth(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveMaxHealth(UEffect* CurEffect, AActor* owner, AActor* other);
	static void AddProtection(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveProtection(UEffect* CurEffect, AActor* owner, AActor* other);

	static void AddWeapon(UEffect* CurEffect, AActor* owner, AActor* other);
	static void EffAddEffectsToWeapon(UEffect* CurEffect, AActor* owner, AActor* other);
	static void AddProjectile(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveProjectile(UEffect* CurEffect, AActor* owner, AActor* other);
	static void AddMagazine(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveMagazine(UEffect* CurEffect, AActor* owner, AActor* other);

	static void AddParalisys(UEffect* CurEffect, AActor* owner, AActor* other);
	static void RemoveParalisys(UEffect* CurEffect, AActor* owner, AActor* other);

	static void Repuls(UEffect* CurEffect, AActor* owner, AActor* other);

	static void AddCoins(UEffect* CurEffect, AActor* owner, AActor* other);
};
