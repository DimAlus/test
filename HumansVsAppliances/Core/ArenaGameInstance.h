// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../FuncLib/ArenaCommonTypes.h"
#include "../Effects/Effect.h"
#include "HumansVsAppliances/IdleEffectUser/IdleEffectUser.h"
//#include "Dom/JsonObject.h"
#include "ArenaGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FApplyedSkill
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int skillIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int level; // Who use skill
};


USTRUCT(BlueprintType)
struct FStringArray {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> strings;
};
USTRUCT(BlueprintType)
struct FEnemyDropDataArrayArray {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FEnemyDropDataArray> datas;
};

/**
 * 
 */
UCLASS()
class HUMANSVSAPPLIANCES_API UArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void DebugString(const FString& debugger);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSetting")
	UDataTable* WeaponInfoTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSetting")
	UDataTable* LevelInfoTable = nullptr;
	
	UFUNCTION(BlueprintCallable)
	bool GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	TArray<FApplyedSkill> SkillApplyes { { 20, 1 }, { 31, 1 } }; //NOT USED!!!

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	int Coins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	EDifficultyLevel CurDiffLevel = EDifficultyLevel::Simple;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	float MainSoundVolume{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	float EffectSoundVolume{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DB)
	TArray<FString> CompletedLevels{};
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UDataTable* SkillTreeTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UDataTable* SkillApplyedTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	UDataTable* SkillConstTable = nullptr;*/

	UFUNCTION(BlueprintCallable)
	bool GetLevelInfoByName(FName NameLevel, FLevelInfo& OutInfo);
	

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Levels)
	bool IsEnvironment(AActor* other);




	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TablePaths)
	FString ConstEffectsFile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TablePaths)
	FString SkillTableFile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TablePaths)
	FString TagsFile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TablePaths)
	FString ConflictsFile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TablePaths)
	FString DropsFile;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TablePaths)
	TMap<TSubclassOf<AActor>, FString> ClassNames;
	UPROPERTY()
	TMap<int, int> BuyedSkills{ { 20, 1 }, { 31, 1 }, {51, 1} };
	UPROPERTY()
	TMap<FString, FStringArray> DT_ConstTags;
	UPROPERTY()
	TMap<FString, FEffectMetaArray> DT_Tags;
	UPROPERTY()
	TMap<int, FSkillTreeElem> DT_TreeSkills;
	UPROPERTY()
	TMap<FString, FEnemyDropDataArrayArray> DT_Drops; // classname -> drops
	UPROPERTY()
	TArray<FStringArray> DT_Conflicts;

	UPROPERTY()
	TMap<FString, FEffectMetaArray> CashSkills;//class -> metas


	// int - int  buyed skills
	// tag - skill const skills
	// skill in tree
	// conflicted skills
	//// tag set -- in cash
	// tag - all skills cash
	// class2tags
		
	float SkillsLoaded = false;

	bool GetJsonFromFile(FString fname, TSharedPtr<FJsonObject>& obj) const;
	void SetJsonToFile(FString fname, TSharedPtr<FJsonObject> obj) const;

	void LoadJsonConsts(TSharedPtr<FJsonObject> json);
	void LoadJsonTree(TSharedPtr<FJsonObject> json);
	void LoadJsonConflicts(TSharedPtr<FJsonObject> json);
	void LoadJsonTags(TSharedPtr<FJsonObject> json);
	void LoadJsonDrops(TSharedPtr<FJsonObject> json);

	
	FDBStruct CurDBJson{};
	static FString GetPathToDB();
	UFUNCTION(BlueprintCallable, Category = DB)
	bool LoadDB();
	UFUNCTION(BlueprintCallable, Category = DB)
	void SaveDB();

	UFUNCTION()
	void LoadSkillsFromFile();
	//getting effects to add on object by tags
	UFUNCTION(BlueprintCallable, Category = Effects)
	TArray<UEffectsMeta*> GetEffectsToApply(TSubclassOf<AActor> ActorClass);
	UFUNCTION(BlueprintCallable, Category = Effects)
	TArray<UEffectsMeta*> GetEffectsToApplyByClass(FString ClassName);
	UFUNCTION(BlueprintCallable, Category = Effects)
	TArray<UEffectsMeta*> GetEffectsToApplyByTags(TArray<FString> tags);
	UFUNCTION(BlueprintCallable, Category = Effects)
	TArray<FEnemyDropData> GetDropEffectsToPlayer(TSubclassOf<AActor> ActorClass);
	//getting effects, wich player buy
	/*UFUNCTION()
	TMap<FString, FEffectMetaArray> GetBuyedSkills();*/
	UFUNCTION()
	TArray<FString> GetBuyedTagsByClassName(FString cls);
	UFUNCTION(BlueprintCallable, Category = Effects)
	FSkillTreeElem FindSkillByIndex(int index); //from tree skills
	UFUNCTION(BlueprintCallable, Category = Effects)
		void ResetSkillsLevels();
	UFUNCTION(BlueprintCallable, Category = Effects)
		int GetLevelOfSkill(int index);
	UFUNCTION(BlueprintCallable, Category = Effects)
		int GetMaxLevelOfSkill(int index);
	UFUNCTION(BlueprintCallable, Category = Effects)
		void ChangeBuyedLevel(int index, int level);
	UFUNCTION()
		void CalculateEffectsToApply(FString Tag);
	UFUNCTION()
		void DropConflictedTags(TArray<FString>& tagArray);
	UFUNCTION()
		void DropSomeEffects(FEffectMetaArray& metaArray);
	UFUNCTION(BlueprintCallable, Category = Effects)
		void ClearSkillCash();
	UFUNCTION()
		//TSubclassOf<AActor> GetClassFromName(FString classname);
	FString GetNameByClass(TSubclassOf<AActor> ActorClass);
};
