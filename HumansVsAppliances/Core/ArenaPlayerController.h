// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PlayerPawn/ArenaPlayerPawn.h"
#include "HumansVsAppliances/IdleEffectUser/IdleEffectUser.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Texture2D.h"
#include "ArenaPlayerController.generated.h"




DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeCountLives , int, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeTotalScore , int, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRespawnPlayerCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndGame);
/**
 * 
 */
UCLASS()
class HUMANSVSAPPLIANCES_API AArenaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = SurfacePlatform)
	TSubclassOf<class AActor> ClassSurfacePlatform = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = AICharacter)
	TSubclassOf<class AEnemyPawn> ClassEnemiesRouter = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = AICharacter)
	TSubclassOf<class AEnemyPawn> ClassEnemiesWashingMachine = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = AICharacter)
	TSubclassOf<class AEnemyPawn> ClassEnemiesRoboCleaner = nullptr;
	
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = Health)
	FOnChangeCountLives OnChangeCountLives;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = Health)
	FOnRespawnPlayerCharacter OnRespawnPlayerCharacter;
 
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = Score)
	FOnChangeTotalScore OnChangeTotalScore;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = GameState)
	FOnEndGame OnEndGame;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	AArenaPlayerPawn* CurrentPlayerCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Health)
	int CountPlayerLives = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemies)
	int MaxLivingEnemy{50};
	
	UPROPERTY()
	AActor* SurfacePlatform = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	TSubclassOf<AActor> SpawnerClass;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float SurfaceXfactor = 200;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float SurfaceYfactor = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn)
	int BeginRespawnCountEnemies = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn)
	int IncRespawnCountEnemies = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Respawn)
	float TimeToRespawnEnemies = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float ChanceSpawnRouter = 45;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float ChanceSpawnRoboCleaner = 35;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	float ChanceSpawnWashingMachine= 20;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Respawn)
	FTimerHandle TimerHandle_Respawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Enemies)
	UDataTable* CurEnemiesInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Level)
	UDataTable* CurLevelInfo;
	
	bool bIsStartGame = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kills)
	int CountKilledRouters = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kills)
	int CountKilledRoboWacuums = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kills)
	int CountKilledWashers = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kills)
	int TotalKills = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Kills)
	int TowerKills = 0;

	UFUNCTION()
	void SpawnDirection();
	UFUNCTION(BlueprintCallable, Category = EnemyDrop)
	void EnemyDeadEvent(AActor* actor);
	UFUNCTION(BlueprintCallable, Category = EnemyDrop)
	void SpawnDrop(TSubclassOf<AIdleEffectUser> DropClass, FVector Aim);
	UFUNCTION(BlueprintCallable, Category = EnemyDrop)
	TArray<FEnemyDropData> GetDropRandEffects(TSubclassOf<AIdleEffectUser> DropClass);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Drop)
	TArray<FEnemyDrop> drops;
	UPROPERTY(EditDefaultsOnly, Category = Respawn)
	TSubclassOf<AActor> directionClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Drop)
	TArray<FEnemyDropChoiser> dropRandomizer;
protected:
	float sumdrops = -1;

	UFUNCTION()
	void CharacterDead();

	void RespawnPlayerCharacter();
	UFUNCTION()
	void RespawnPlayerCharacterImpl(TSubclassOf<AActor> ActorClass, FVector locaion);
	
	void SpawnEnemyActor();
	UFUNCTION()
	void SpawnEnemyActorImpl(TSubclassOf<AActor> ActorClass, FVector locaion);

	static int GetRandRange(int Min, int Max);

	AActor* direction;

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	UTexture2D* GetIconByClass(TSubclassOf<AActor> actorClass);

	UFUNCTION(BlueprintCallable)
	void NotifyFromEnemy(EEnemyType CurType);
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* CurPawn) override;

	UFUNCTION(BlueprintCallable, Category = Respawn)
	void SetSpawnPlatform();

	UFUNCTION(BlueprintCallable, Category = Levels)
	TArray<FString> GetCSVFile(FString Path);
	
	UFUNCTION(BlueprintCallable, Category = Enemies)
	UDataTable* LoadCSVDataFile(ECSVTableType CurTableType);

	UFUNCTION(BlueprintCallable, Category = Levels)
	void LoadRow(UDataTable* CurDataTable, FString RowName, FLevelInfo& OutInfo);


	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Levels)
	void AddDamageToViewport(AActor* other, float damage);

	
};
