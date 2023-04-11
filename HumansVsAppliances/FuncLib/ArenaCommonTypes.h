// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "HumansVsAppliances/Effects/Effect.h"
#include "ArenaCommonTypes.generated.h"

UENUM(BlueprintType)
enum class ESides : uint8
{
	Front UMETA(DisplayName = Front),
	Back UMETA(DisplayName = Back),
	Left UMETA(DisplayName = Left),
	Right UMETA(DisplayName = Right)
};
UENUM(BlueprintType)
enum class ECSVTableType : uint8
{
	Levels UMETA(DisplayName = Levels),
	Enemies UMETA(DisplayName = Enemies)
};

UENUM(BlueprintType)
enum class EDifficultyLevel : uint8
{
	Simple UMETA(DisplayName = Simple),
	Middle UMETA(DisplayName = Middle),
	Hard UMETA(DisplayName = Hard)
};

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Router UMETA(DisplayName = Router),
	Washer UMETA(DisplayName = Washer),
	RobotVacuum UMETA(DisplayName = RobotVacuum)
};


UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	Simple UMETA(DisplayName = Simple),
	Rocket UMETA(DisplayName = Rocket)
};

UENUM(BlueprintType)
enum class EOwnerType : uint8
{
	Player UMETA(DisplayName = Player),
	Enemy UMETA(DisplayName = Enemy)
};


USTRUCT(BlueprintType)
struct FDBStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	int CurDiffLevel{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	int Coins{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	float MainSoundVolume{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	float EffectSoundVolume{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	TMap<int, int> BuyedSkills{ { 20, 1 }, { 31, 1 }, {51, 1} };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	TArray<FString> CompletedLevels{};
	
};


USTRUCT(BlueprintType)
struct FProjectileSetting
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	TSubclassOf<class AArenaProjectile> Projectile = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	UStaticMeshComponent* ProjectileStaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= ProjectileSetting)
	EProjectileType ProjectileType = EProjectileType::Simple;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	float InitialSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	float MaxSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	float ProjectileLifeTime = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	FVector VectorOfClimb = FVector(0, 0, 500);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	float TimeOfClimb = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	UParticleSystem* ProjectileTrailFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	UParticleSystem* ExploseFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	USoundBase* ExploseSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	TSubclassOf<class AActor> SpawnObject = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	float ChanceToSpawn = 0.3f;

	EOwnerType OwnerType;
};


USTRUCT(BlueprintType)
struct FFiringAnimationInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CharAnim)
	UAnimMontage* AnimCharFire = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponAnim)
	UAnimMontage* AnimWeaponFire = nullptr;
	
};



UENUM(BlueprintType)
enum class EShootingType : uint8
{
	Auto UMETA(DisplayName = Auto),
	OnceAuto UMETA(DisplayName = OnceAuto),
	Once UMETA(DisplayName = Once)
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol			UMETA(DisplayName = Pistol),
	Rifle			UMETA(DisplayName = Rifle),
	MachineGun		UMETA(DisplayName = MachineGun),
	BreadLauncher	UMETA(DisplayName = BreadLauncher),
	BurntBreadLauncher	UMETA(DisplayName = BurntBreadLauncher),
	RocketLauncher	UMETA(DisplayName = RocketLauncher),
	LineLaser		UMETA(DisplayName = LineLaser),
	CircleLaser		UMETA(DisplayName = CircleLaser),
	Jumping			UMETA(DisplayName = Jumping),
	AcidWeapon		UMETA(DisplayName = AcidWeapon),
	MeleeWeapon		UMETA(DisplayName = MeleeWeapon)
};


USTRUCT(BlueprintType)
struct FWeaponInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Class)
	TSubclassOf<class AArenaWeapon> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	EWeaponType WeaponType = EWeaponType::Pistol;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	EShootingType ShootingType = EShootingType::Auto;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float RateOfFire = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	int ClipVolume = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	int32 NumberProjectileByShot = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float DamageFactor = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float MaxDamageDistance = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float MinDamageDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Location)
	FName NameAttachSocket = "SocketRightHand";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	FProjectileSetting ProjectileSetting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdditionalData)
	FVector TargetVector_1 = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdditionalData)
	float TargetFloat_1 = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdditionalData)
	FVector TargetVector_2 = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AdditionalData)
	float TargetFloat_2 = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* SoundFireWeapon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	UParticleSystem* EffectFireWeapon = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HitDecal)
	UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FFiringAnimationInfo AnimationOfFiring;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory)
	UTexture2D* WeaponIcon = nullptr;
	
};

UENUM(BlueprintType)
enum class EIdleEffectUserType : uint8
{
	PICKED			UMETA(DisplayName = "PICKED")
	, ACTIVE		UMETA(DisplayName = "ACTIVE")
};
USTRUCT(BlueprintType)
struct FIdleEffectUserInfo 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	EIdleEffectUserType IdolType = EIdleEffectUserType::PICKED;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float TimeLife = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float Radius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float RateOfFire = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	int ClipVolume = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	int RechargeTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundBase* SoundFireWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FX)
	UParticleSystem* EffectFireWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HitDecal)
	UDecalComponent* DecalOnHit = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	FFiringAnimationInfo AnimationOfFiring;

};


USTRUCT(BlueprintType)
struct FWeaponTestedInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float RateOfFire = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	int ClipVolume = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float MaxDamageDistance = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Specifications)
	float MinDamageDistance = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	float ProjectileInitialSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProjectileSetting)
	float ProjectileMaxSpeed = 0.0f;
	
};


USTRUCT(BlueprintType)
struct FPawnSpeed
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float WalkSpeed = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float SprintSpeed = 1800.0f;
};


USTRUCT(BlueprintType)
struct FSkill
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> Parent; // Who use skill

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString comment;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UEffectsMeta*> effects;
};


USTRUCT(BlueprintType)
struct FSkillTreeElement
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int index;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> parents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkill> levels;

};


USTRUCT(BlueprintType)
struct FEnemiesInfo : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	int HealthInPercent = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed)
	int MaxSpeed = 490;
};


USTRUCT(BlueprintType)
struct FLevelInfo : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Level)
	int LevelTimeSec = 1800;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RespawnEnemies)
	int BeginRespawnCountEnemies = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RespawnEnemies)
	int IncRespawnCountEnemies = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RespawnEnemies)
	float TimeToRespawnEnemies = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float ChanceSpawnRouter = 45;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float ChanceSpawnRoboCleaner = 35;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float ChanceSpawnWashingMachine= 20;
};



UCLASS()
class HUMANSVSAPPLIANCES_API UArenaCommonTypes: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	static AArenaWeapon* SetWeapon(UWorld* CurUWorld,
								   USkeletalMeshComponent* TargetMesh,
								   AActor* Owner,
								   APawn* Instigator,
								   APlayerController* CurPlayerController,
								   const FName NameWeapon);

	UFUNCTION(BlueprintCallable)
	static void RepulseEnemy(AEnemyPawn* EnemyPawn, FVector DirToRepulsion);
};

