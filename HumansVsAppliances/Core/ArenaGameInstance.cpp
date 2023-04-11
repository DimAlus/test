// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaGameInstance.h"

#include "JsonObjectConverter.h"
#include "Engine/DataTable.h"
#include "HumansVsAppliances/FuncLib/ArenaCommonTypes.h"
#include "HumansVsAppliances/Weapons/ArenaWeapon.h"


#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"


bool UArenaGameInstance::GetWeaponInfoByName(const FName NameWeapon, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	if (WeaponInfoTable)
	{
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UArenaGameInstance::GetWeaponInfoByName - Weapon table -NULL"));
	}
	
	return bIsFind;
}

bool UArenaGameInstance::GetLevelInfoByName(FName NameLevel, FLevelInfo& OutInfo)
{
	bool bIsFind = false;
	FLevelInfo* LevelInfoRow;
	
	if (LevelInfoTable)
	{
		LevelInfoRow = LevelInfoTable->FindRow<FLevelInfo>(NameLevel, "", false);
		if (LevelInfoRow)
		{
			bIsFind = true;
			OutInfo = *LevelInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UArenaGameInstance::Level table -NULL"));
	}
	return bIsFind;
}









EEffectAim AimNameToAim(FString name) {
	if (name == "PLAYER")
		return EEffectAim::PLAYER;
	if (name == "ENEMY")
		return EEffectAim::ENEMY;
	return EEffectAim::ALL;

}
EEffectTypes EffectNameToType(FString name) {
	static TMap<FString, EEffectTypes> en = {
		{"MOMENTAL_DAMAGE", EEffectTypes::MOMENTAL_DAMAGE},
		{"PERIODIC_DAMAGE", EEffectTypes::PERIODIC_DAMAGE},
		{"HEAL", EEffectTypes::HEAL},
		{"PARALISYS", EEffectTypes::PARALISYS},
		{"REPULSION", EEffectTypes::REPULSION},
		{"ADD_EFFECTS", EEffectTypes::ADD_EFFECTS},
		{"UP_ALL_EFFECTS", EEffectTypes::UP_ALL_EFFECTS},
		{"UP_MOMENTAL_DAMAGE", EEffectTypes::UP_MOMENTAL_DAMAGE},
		{"UP_PERIODIC_DAMAGE", EEffectTypes::UP_PERIODIC_DAMAGE},
		{"UP_HEAL", EEffectTypes::UP_HEAL},
		{"UP_PARALISYS", EEffectTypes::UP_PARALISYS},
		{"MAX_HELTH", EEffectTypes::MAX_HELTH},
		{"SPEED", EEffectTypes::SPEED},
		{"PROTECTION", EEffectTypes::PROTECTION},
		{"ADD_PROJECTILE", EEffectTypes::ADD_PROJECTILE},
		{"ADD_MAGAZINE_SIZE", EEffectTypes::ADD_MAGAZINE_SIZE},
		{"ADD_TO_WEAPON", EEffectTypes::ADD_TO_WEAPON},
		{"WEAPON_PISTOL", EEffectTypes::WEAPON_PISTOL},
		{"WEAPON_RIFLE", EEffectTypes::WEAPON_RIFLE},
		{"WEAPON_MACHINEGUN", EEffectTypes::WEAPON_MACHINEGUN},
		{"WEAPON_LINE_REPULS", EEffectTypes::WEAPON_LINE_REPULS},
		{"WEAPON_CIRC_REPULS", EEffectTypes::WEAPON_CIRC_REPULS},
		{"WEAPON_BREAD", EEffectTypes::WEAPON_BREAD},
		{"WEAPON_BURNT_BREAD", EEffectTypes::WEAPON_BURNT_BREAD},
		{"WEAPON_ROCKET", EEffectTypes::WEAPON_ROCKET},
		{"WEAPON_JUMPING", EEffectTypes::WEAPON_JUMPING},
		{"WEAPON_MELEE", EEffectTypes::WEAPON_MELEE},
		{"WEAPON_ACID", EEffectTypes::WEAPON_ACID},
		{"ADD_COINS", EEffectTypes::ADD_COINS}
	};
	if (en.Contains(name))
		return en[name];
	return EEffectTypes::NONE;
}





UEffectsMeta* JsonObjectToMeta(FJsonObject* json, FString Name) {
	TArray<FString> keys;
	json->Values.GetKeys(keys);


	UEffectsMeta* meta = NewObject<UEffectsMeta>();
	meta->effectType = EffectNameToType(Name);

	bool isUp = UEffect::GetTypeOfType(meta->effectType) == ETepesEffectType::UP_EFFECT;

	meta->effectAim = AimNameToAim(keys.Contains("AIM") ? json->GetStringField("AIM") : "ALL");
	meta->power = keys.Contains("POWER") ? json->GetNumberField("POWER") : (isUp ? 1 : 0);
	meta->radius = keys.Contains("RADIUS") ? json->GetNumberField("RADIUS") : (isUp ? 1 : 0);
	meta->radianalPower = keys.Contains("RPOWER") ? json->GetNumberField("RPOWER") : (isUp ? 1 : 0);
	meta->period = keys.Contains("PERIOD") ? json->GetNumberField("PERIOD") : (isUp ? 1 : 0);
	meta->time = keys.Contains("TIME") ? json->GetNumberField("TIME") : (isUp ? 1 : 0);
	
	if (keys.Contains("LINKED")) {
		const TSharedPtr<FJsonObject>& obj = json->GetObjectField("LINKED");

		TArray<FString> effnames;
		obj.Get()->Values.GetKeys(effnames);
		for (int j = 0; j < effnames.Num(); j++) {
			const TSharedPtr<FJsonObject>& inner = obj.Get()->GetObjectField(effnames[j]);
			meta->linkedEffects.Add(JsonObjectToMeta(inner.Get(), effnames[j]));
		}
	}

	return meta;
}




void UArenaGameInstance::LoadJsonConsts(TSharedPtr<FJsonObject> json) {
	DT_ConstTags.Reset();

	TArray<FString> keys;
	json->Values.GetKeys(keys);
	for (int i = 0; i < keys.Num(); i++) {
		const TArray<TSharedPtr<FJsonValue>>& arr = json->GetArrayField(keys[i]);
		DT_ConstTags.Add(TTuple<FString, FStringArray>(keys[i], FStringArray()));
		for (int j = 0; j < arr.Num(); j++)
			DT_ConstTags[keys[i]].strings.Add(arr[j].Get()->AsString());
	}
}
void UArenaGameInstance::LoadJsonTree(TSharedPtr<FJsonObject> json) {
	DT_TreeSkills.Reset();

	TArray<FString> keys;
	json->Values.GetKeys(keys);
	for (int i = 0; i < keys.Num(); i++) {
		const TSharedPtr<FJsonObject>& elem = json->GetObjectField(keys[i]);
		int index = FCString::Atoi(*keys[i]);
		DT_TreeSkills.Add(TTuple<int, FSkillTreeElem>(index, FSkillTreeElem()));
		DT_TreeSkills[index].index = index;

		DT_TreeSkills[index].name = elem.Get()->GetStringField("NAME");

		DT_TreeSkills[index].comment = elem.Get()->GetStringField("COMMENT");

		const TArray<TSharedPtr<FJsonValue>>& prntptr = elem.Get()->GetArrayField("PARENTS");
		for (int f = 0; f < prntptr.Num(); f++)
			DT_TreeSkills[index].parents.Add(prntptr[f].Get()->AsNumber());

		const TArray<TSharedPtr<FJsonValue>>& prcsptr = elem.Get()->GetArrayField("PRICES");
		for (int f = 0; f < prcsptr.Num(); f++)
			DT_TreeSkills[index].prices.Add(prcsptr[f].Get()->AsNumber());

		TArray<TSharedPtr<FJsonValue>> lvlarr;
		const TArray<TSharedPtr<FJsonValue>>& lvlsptr = elem.Get()->GetArrayField("LEVELS");// = &lvlarr;
		
		//if (elem.Get()->TryGetArrayField("LEVELS", lvlsptr)) {
			
			for (int f = 0; f < lvlsptr.Num(); f++) {
				const TSharedPtr<FJsonObject> lvl = lvlsptr[f].Get()->AsObject();
				DT_TreeSkills[index].levels.Add(FSkillTreeLevel());
				FSkillTreeLevel& lv = DT_TreeSkills[index].levels[f];

				lv.comment = lvl.Get()->GetStringField("COMMENT");

				lv.owner = lvl.Get()->GetStringField("CLASS");

				lv.range = lvl.Get()->GetIntegerField("RANGE");
				

				//TArray<TSharedPtr<FJsonValue>> tagsarr;
				const TArray<TSharedPtr<FJsonValue>>& tagsptr = lvl.Get()->GetArrayField("TAGS");
				
				for (int ff = 0; ff < tagsptr.Num(); ff++)
					lv.tags.Add(tagsptr[ff].Get()->AsString());
			}
		//}
	}
}
void UArenaGameInstance::LoadJsonConflicts(TSharedPtr<FJsonObject> json) {
	DT_Conflicts.Reset();

	TArray<FString> keys;
	json->Values.GetKeys(keys);
	for (int i = 0; i < keys.Num(); i++) {
		const TArray<TSharedPtr<FJsonValue>>& arr = json->GetArrayField(keys[i]);
		for (int j = 0; j < arr.Num(); j++) {
			const TArray<TSharedPtr<FJsonValue>>& a = arr[j].Get()->AsArray();
			DT_Conflicts.Add(FStringArray());
			for (int k = 0; k < a.Num(); k++)
				DT_Conflicts[j].strings.Add(a[k].Get()->AsString());
		}
	}
}
void UArenaGameInstance::LoadJsonTags(TSharedPtr<FJsonObject> json) {
	DT_Tags.Reset();
	
	TArray<FString> tags;
	json->Values.GetKeys(tags);
	for (int i = 0; i < tags.Num(); i++) {
		const TSharedPtr<FJsonObject>& obj = json->GetObjectField(tags[i]);
		DT_Tags.Add(TTuple<FString, FEffectMetaArray>(tags[i], FEffectMetaArray()));

		TArray<FString> effnames;
		obj.Get()->Values.GetKeys(effnames);
		for (int j = 0; j < effnames.Num(); j++) {
			const TSharedPtr<FJsonObject>& inner = obj.Get()->GetObjectField(effnames[j]);
			DT_Tags[tags[i]].metas.Add(JsonObjectToMeta(inner.Get(), effnames[j]));
		}

	}
}
void UArenaGameInstance::LoadJsonDrops(TSharedPtr<FJsonObject> json) {
	DT_Drops.Reset();

	TArray<FString> keys;
	json->Values.GetKeys(keys);
	for (int i = 0; i < keys.Num(); i++) {
		const TArray<TSharedPtr<FJsonValue>>& arr = json->GetArrayField(keys[i]);
		DT_Drops.Add(TTuple<FString, FEnemyDropDataArrayArray>(keys[i], FEnemyDropDataArrayArray()));
		FEnemyDropDataArrayArray& daa = DT_Drops[keys[i]];
		TMap<int, int> ranges;

		for (int j = 0; j < arr.Num(); j++) {
			const TSharedPtr<FJsonObject>& obj = arr[j].Get()->AsObject();
			TArray<FString> vals;
			obj.Get()->Values.GetKeys(vals);
			FEnemyDropData data;
			int indx = 0;
			if (vals.Contains("RANGE"))
				indx = obj.Get()->GetIntegerField("RANGE");
			if (!ranges.Contains(indx)) {
				ranges.Add(TTuple<int, int>(indx, daa.datas.Num()));
				daa.datas.Add(FEnemyDropDataArray());
			}

			data.chance = vals.Contains("CHANCE") ? obj.Get()->GetNumberField("CHANCE") : 1;
			data.comment = vals.Contains("COMMENT") ? obj.Get()->GetStringField("COMMENT") : "";
			data.name = vals.Contains("NAME") ? obj.Get()->GetStringField("NAME") : "";
			FString tp = vals.Contains("TYPE") ? obj.Get()->GetStringField("TYPE") : "";
			if (tp == "RED")
				data.Type = EDropType::RED;
			else if (tp == "ORANGE")
				data.Type = EDropType::ORANGE;
			else if (tp == "GREEN")
				data.Type = EDropType::GREEN;
			else data.Type = EDropType::COINS;

			if (vals.Contains("TAGS")) {
				const TArray<TSharedPtr<FJsonValue>>& tags = obj.Get()->GetArrayField("TAGS");
				for (int k = 0; k < tags.Num(); k++) {
					data.DropTags.Add(tags[k].Get()->AsString());
				}
			}
			daa.datas[ranges[indx]].datas.Add(data);
		}

	}
}

FString UArenaGameInstance::GetPathToDB()
{
	FString Path = FPaths::ProjectDir();
	Path.Append("Tables");
	Path.Append("/db.json");
	return Path;
}

void UArenaGameInstance::SaveDB()
{

	FString SerData;
	
	CurDBJson.CurDiffLevel = static_cast<int>(CurDiffLevel);
	CurDBJson.Coins = Coins;
	CurDBJson.MainSoundVolume = MainSoundVolume;
	CurDBJson.EffectSoundVolume = EffectSoundVolume;
	CurDBJson.BuyedSkills = BuyedSkills;
	CurDBJson.CompletedLevels = CompletedLevels;

		
	FJsonObjectConverter::UStructToJsonObjectString(CurDBJson, SerData);
	FFileHelper::SaveStringToFile(SerData, ToCStr(GetPathToDB()));
}

bool UArenaGameInstance::LoadDB()
{
	bool Res = true;
	FString SerData;
	FFileHelper::LoadFileToString(SerData, ToCStr(GetPathToDB()));

	if(!FJsonObjectConverter::JsonObjectStringToUStruct(SerData, &CurDBJson, 0, 0))
	{
		Res = false;
	}
	else
	{
		CurDiffLevel = static_cast<EDifficultyLevel>(CurDBJson.CurDiffLevel);
		Coins = CurDBJson.Coins;
		MainSoundVolume = CurDBJson.MainSoundVolume;
		EffectSoundVolume = CurDBJson.EffectSoundVolume;
		BuyedSkills = CurDBJson.BuyedSkills;
		CompletedLevels = CurDBJson.CompletedLevels;
	}
	return Res;
}

bool UArenaGameInstance::GetJsonFromFile(FString fname, TSharedPtr<FJsonObject>& obj) const
{
	FString file;
	FFileHelper::LoadFileToString(file, *fname);
	
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(file);
	
	//TSharedPtr<FJsonObject> JsonObject;
	bool f = FJsonSerializer::Deserialize(*Reader, obj);
	//if (f) obj = JsonObject;
	return f;
	//return 1;
}

void UArenaGameInstance::SetJsonToFile(FString fname, TSharedPtr<FJsonObject> obj) const
{
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&fname);
	FJsonSerializer::Serialize(obj.ToSharedRef(), Writer);
}

void UArenaGameInstance::LoadSkillsFromFile() {
	FString Path = FPaths::ProjectDir();
	Path.Append("Tables/");
	FString DiffPath;
	switch (CurDiffLevel)
	{
	case EDifficultyLevel::Simple:
		DiffPath = "Simple/";
		break;
	case EDifficultyLevel::Middle:
		DiffPath = "Middle/";
		break;
	case EDifficultyLevel::Hard:
		DiffPath = "Hard/";
		break;
	default:
		break;
	}

	TSharedPtr<FJsonObject> jconst;
	TSharedPtr<FJsonObject> jstab;
	TSharedPtr<FJsonObject> jtags;
	TSharedPtr<FJsonObject> jdrops;
	TSharedPtr<FJsonObject> jconflicts;

	if (!(GetJsonFromFile(Path + DiffPath + "ConstEffects.json", jconst) &&
		GetJsonFromFile(Path + DiffPath + "SkillTree.json", jstab)&&
		GetJsonFromFile(Path + "Tags.json", jtags)&&
		GetJsonFromFile(Path + DiffPath + "Drops.json", jdrops)&&
		GetJsonFromFile(Path + "Conflicts.json", jconflicts)))
		return;

	LoadJsonTags(jtags);
	LoadJsonConflicts(jconflicts);
	LoadJsonConsts(jconst);
	LoadJsonTree(jstab);
	LoadJsonDrops(jdrops);
	LoadDB();
	SkillsLoaded = true;
}
TArray<UEffectsMeta*> UArenaGameInstance::GetEffectsToApplyByTags(TArray<FString> tags) { //without drop
	TArray<UEffectsMeta*> res;
	for (int i = 0; i < tags.Num(); i++)
		if (DT_Tags.Contains(tags[i])) {
			for (int j = 0; j < DT_Tags[tags[i]].metas.Num(); j++) {
				UEffectsMeta* meta = NewObject< UEffectsMeta>();
				meta->Copy(DT_Tags[tags[i]].metas[j]);
				res.Add(meta);
			}

			//res.metas.Append(DT_Tags[tags[i]].metas);
		}
	return res;
}
TArray<UEffectsMeta*> UArenaGameInstance::GetEffectsToApplyByClass(FString ClassName) {
	if (!SkillsLoaded) {
		LoadSkillsFromFile();
	}
	if (!CashSkills.Contains(ClassName)) {
		CalculateEffectsToApply(ClassName);
	}
	return CashSkills[ClassName].metas;
	/*TArray<UEffectsMeta> res;
	for (int i = 0; i < Tags.Num(); i++) {
		if (!CashSkills.Contains(Tags[i]))
			CalculateEffectsToApply(Tags[i]);
		res.Append(CashSkills[Tags[i]].metas);
	}
	return res;*/
}
TArray<UEffectsMeta*> UArenaGameInstance::GetEffectsToApply(TSubclassOf<AActor> ActorClass) {
	if (!SkillsLoaded) {
		LoadSkillsFromFile();
	}
	return GetEffectsToApplyByClass(GetNameByClass(ActorClass));
}
int GetOneOfData(TArray<FEnemyDropData>& datas) {
	float chs = 0;
	for (int i = 0; i < datas.Num(); i++)
		chs += datas[i].chance;
	if (chs < 0.1) return -1;
	chs = float(rand()) / RAND_MAX * chs;
	for (int i = 0; i < datas.Num(); i++) {
		chs -= datas[i].chance;
		if (chs <= 0)
			return i;
	}
	return datas.Num() - 1;		
}
TArray<FEnemyDropData> UArenaGameInstance::GetDropEffectsToPlayer(TSubclassOf<AActor> ActorClass) {
	if (!SkillsLoaded) {
		LoadSkillsFromFile();
	}
	TArray<FEnemyDropData> res;
	FString name = GetNameByClass(ActorClass);
	if (!DT_Drops.Contains(name))
		return res;
	FEnemyDropDataArrayArray& drops = DT_Drops[name];
	for (int i = 0; i < drops.datas.Num(); i++) {
		int indx = GetOneOfData(drops.datas[i].datas);
		if (indx >= 0)
			res.Add(drops.datas[i].datas[indx]);
	}
	return res;
}/*
TMap<FString, FEffectMetaArray> UArenaGameInstance::GetBuyedSkills() {
	TMap<FString, FEffectMetaArray> res;
	TArray<int> inds;
	BuyedSkills.GetKeys(inds);
	for (int i = 0; i < inds.Num(); i++) {
		if (!TreeSkills.Contains(inds[i]))
			continue;
		TArray<FSkillTreeLevel>& lvls = TreeSkills[inds[i]].levels;
		int up = BuyedSkills[inds[i]];
		for (int j = 0; j < lvls.Num(); j++)
			if (lvls[j].range <= up) {
				for (int k = 0; k < lvls[j].skills.Num(); k++) {
					FString tag = lvls[j].skills[k].Tag;
					TArray<UEffectsMeta>& metas = lvls[j].skills[k].effects;
					if (!res.Contains(tag))
						res.Add(TTuple<FString, FEffectMetaArray>(tag, FEffectMetaArray()));
					res[tag].metas.Append(metas);
				}
			}
	}

	return res;
}*/
TArray<FString> UArenaGameInstance::GetBuyedTagsByClassName(FString cls) {
	TArray<FString> res;
	TArray<int> inds;
	BuyedSkills.GetKeys(inds);
	for (int i = 0; i < inds.Num(); i++) {
		if (!DT_TreeSkills.Contains(inds[i]))
			continue;
		TArray<FSkillTreeLevel>& lvls = DT_TreeSkills[inds[i]].levels;
		int up = BuyedSkills[inds[i]];
		for (int j = 0; j < lvls.Num(); j++)
			if (lvls[j].range < up && lvls[j].owner == cls) 
				res.Append(lvls[j].tags);
				//for (int k = 0; k < lvls[j].skills.Num(); k++) 
				//	if (lvls[j].skills[k].owner == cls)
				//		res.Append(lvls[j].skills[k].effects);
	}

	return res;
}
FSkillTreeElem UArenaGameInstance::FindSkillByIndex(int index) {
	if (!SkillsLoaded) {
		LoadSkillsFromFile();
	}
	if (!DT_TreeSkills.Contains(index))
		return FSkillTreeElem();
	return DT_TreeSkills[index];
}
void UArenaGameInstance::ResetSkillsLevels() {
	BuyedSkills = { { 20, 1 }, { 31, 1 }, {51, 1} };
}
int UArenaGameInstance::GetLevelOfSkill(int index) {
	if (!SkillsLoaded) {
		LoadSkillsFromFile();
	}
	if (!BuyedSkills.Contains(index))
		return 0;
	return BuyedSkills[index];
}
int UArenaGameInstance::GetMaxLevelOfSkill(int index) {
	if (!SkillsLoaded) {
		LoadSkillsFromFile();
	}
	if (!DT_TreeSkills.Contains(index))
		return 0;
	TArray<FSkillTreeLevel>& lvls = DT_TreeSkills[index].levels;
	int max = -1;
	for (int i = 0; i < lvls.Num(); i++) {
		if (lvls[i].range > max)
			max = lvls[i].range;
	}
	return max + 1;
}
void UArenaGameInstance::ChangeBuyedLevel(int index, int level) {
	if (!SkillsLoaded) {
		LoadSkillsFromFile();
	}
	if (!BuyedSkills.Contains(index))
		BuyedSkills.Add(TTuple<int, int>(index, level));
	else if (BuyedSkills[index] != level)
		BuyedSkills[index] = level;
	else return;
	ClearSkillCash();

}
void UArenaGameInstance::CalculateEffectsToApply(FString Cls) {
	TArray<FString> tags;
	if (DT_ConstTags.Contains(Cls)) 
		tags.Append(DT_ConstTags[Cls].strings);
	tags.Append(GetBuyedTagsByClassName(Cls));
	DropConflictedTags(tags);
	
	FEffectMetaArray res;
	
	for (int i = 0; i < tags.Num(); i++)
		if (DT_Tags.Contains(tags[i])) {
			for (int j = 0; j < DT_Tags[tags[i]].metas.Num(); j++) {
				UEffectsMeta* meta = NewObject< UEffectsMeta>();
				meta->Copy(DT_Tags[tags[i]].metas[j]);
				res.metas.Add(meta);
			}

			//res.metas.Append(DT_Tags[tags[i]].metas);
		}
	DropSomeEffects(res);
	CashSkills.Add(TTuple<FString, FEffectMetaArray>(Cls, res));
}
void UArenaGameInstance::DropConflictedTags(TArray<FString>& tagArray) {
	for (int i = 0; i < DT_Conflicts.Num(); i++) {
		TArray<FString>& strs = DT_Conflicts[i].strings;
		bool was = false;
		for (int j = 0; j < strs.Num(); j++)
			for (int k = 0; k < tagArray.Num(); k++)
				if (tagArray[k] == strs[j])
                {
                    if (was) tagArray.RemoveAt(k--);
                    else was = true;
                }
	}

}
void UArenaGameInstance::DropSomeEffects(FEffectMetaArray& metaArray) {
	for (int i = 0; i < metaArray.metas.Num(); i++)
		if (UEffect::GetTypeOfType(metaArray.metas[i]->effectType) == ETepesEffectType::UP_EFFECT) 
			for (int j = i + 1; j < metaArray.metas.Num(); j++)
				if (metaArray.metas[i]->effectType == metaArray.metas[j]->effectType) {
					metaArray.metas[i]->power *= metaArray.metas[j]->power;
					metaArray.metas[i]->radianalPower *= metaArray.metas[j]->radianalPower;
					metaArray.metas[i]->radius *= metaArray.metas[j]->radius;
					metaArray.metas[i]->period *= metaArray.metas[j]->period;
					metaArray.metas[i]->time *= metaArray.metas[j]->time;
					metaArray.metas.RemoveAt(j--);
				}
}
void UArenaGameInstance::ClearSkillCash() {
	CashSkills.Reset();
}
FString UArenaGameInstance::GetNameByClass(TSubclassOf<AActor> ActorClass) {
	if (!ClassNames.Contains(ActorClass)) 
		return FString(); 
	return ClassNames[ActorClass];/*
	FString name(ClassNames[ActorClass]);
	if (!ClassTags.Contains(name))
		return TArray<FString>(); 
	return ClassTags[name].strings;*/
}
