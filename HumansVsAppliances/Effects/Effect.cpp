// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect.h"
#include "EffectInfluenced.h"
#include "EffectUser.h"

void UEffectsMeta::Copy(UEffectsMeta* meta) {
	 this->effectType = meta->effectType;
	 this->effectAim = meta->effectAim;
	 this->power = meta->power;
	 this->radius = meta->radius;
	 this->radianalPower = meta->radianalPower;
	 this->period = meta->period;
	 this->time = meta->time;
	 for (int i = 0; i < meta->linkedEffects.Num(); i++) {
		 linkedEffects.Add(NewObject<UEffectsMeta>());
		 linkedEffects[i]->Copy(meta->linkedEffects[i]);
	 }
}

void UEffect::Copy(UEffect* copy){
	effectAim		= copy->effectAim;
	radianalPower	= copy->radianalPower;
	effectType		= copy->effectType;
	period			= copy->period;
	radius			= copy->radius;
	power			= copy->power;
	time			= copy->time;
	linkedTypes		= copy->linkedTypes;
	linkedEffects	= copy->linkedEffects;

	OnStart			= copy->OnStart;
	OnTick			= copy->OnTick;
	OnEnd			= copy->OnEnd;
	OnCancel		= copy->OnCancel;
	OnBuffEffect	= copy->OnBuffEffect;
	OnDeBuffEffect	= copy->OnDeBuffEffect;
}

void UEffect::Init(UEffectsMeta* meta) {
	effectAim		= meta->effectAim;
	radianalPower	= meta->radianalPower;
	effectType		= meta->effectType;
	period			= meta->period;
	radius			= meta->radius;
	power			= meta->power;
	time			= meta->time;
	linkedTypes		= GetLinkedEffects(meta);
	for (int i = 0; i < meta->linkedEffects.Num(); i++) {
		UEffect* eff = NewObject<UEffect>();
		eff->Init(meta->linkedEffects[i]);
		linkedEffects.Add(eff);
	}

	OnStart			= GetOnStartFunc(meta);
	OnTick			= GetOnTickFunc(meta);
	OnEnd			= GetOnEndFunc(meta);
	OnCancel		= GetOnCancelFunc(meta);
	OnBuffEffect	= GetBuffEffectFunc(meta);
	OnDeBuffEffect	= GetDeBuffEffectFunc(meta);
}

void UEffect::CallStart(AActor* owner, AActor* other){
	if (this->OnStart)
		(this->OnStart)(this, owner, other);
}
void UEffect::CallTick(float deltaTime, AActor* owner, AActor* other){
	if (this->OnTick)
		(this->OnTick)(this, deltaTime, owner, other);
}
void UEffect::CallEnd(AActor* owner, AActor* other){
	if (this->OnEnd)
		(this->OnEnd)(this, owner, other);
}
void UEffect::CallCancel(AActor* owner, AActor* other){
	if (this->OnCancel)
		(this->OnCancel)(this, owner, other);
}
void UEffect::CallBuffEffect(UEffect* effect, AActor* owner){
	if (this->OnBuffEffect)
		(this->OnBuffEffect)(this, effect, owner);
}
void UEffect::CallDeBuffEffect(UEffect* effect, AActor* owner){
	if (this->OnDeBuffEffect)
		(this->OnDeBuffEffect)(this, effect, owner);
}

ETepesEffectType UEffect::GetTypeOfType() {
	return GetTypeOfType(this->effectType);
}

float powerFunc(float max, float min, float point) {
	return min + (max - min)*(-point*point + 1);
}
float UEffect::GetPowerByLength(float rad){
	return rad < 0.1 ? power : radius < rad ? 0 : powerFunc(power, radianalPower, rad / radius);
}
ETepesEffectType UEffect::GetTypeOfType(EEffectTypes tp){
	switch (tp)
	{
	case EEffectTypes::MOMENTAL_DAMAGE:
	case EEffectTypes::PERIODIC_DAMAGE:
	case EEffectTypes::HEAL:
	case EEffectTypes::PARALISYS:
	case EEffectTypes::REPULSION:
	case EEffectTypes::ADD_EFFECTS:
		return ETepesEffectType::INFLUENCE;
	case EEffectTypes::UP_ALL_EFFECTS:
	case EEffectTypes::UP_MOMENTAL_DAMAGE:
	case EEffectTypes::UP_PERIODIC_DAMAGE:
	case EEffectTypes::UP_HEAL:
	case EEffectTypes::UP_PARALISYS:
		return ETepesEffectType::UP_EFFECT;
	case EEffectTypes::MAX_HELTH:
	case EEffectTypes::SPEED:
	default:
		return ETepesEffectType::UP_USER;
	}
}


void (* UEffect::GetOnStartFunc(UEffectsMeta* meta))(UEffect* CurEffect, AActor* owner, AActor* other){
	switch (meta->effectType)
	{
	case EEffectTypes::MOMENTAL_DAMAGE:
		return RemoveHealth;
	case EEffectTypes::PARALISYS:
		return AddParalisys;
	case EEffectTypes::REPULSION:
		return Repuls;
	case EEffectTypes::HEAL:
		return AddHealth;
	case EEffectTypes::ADD_EFFECTS:
		return AddEffectToUser;
	case EEffectTypes::MAX_HELTH:
		return AddMaxHealth;
	case EEffectTypes::PROTECTION:
		return AddProtection;
	case EEffectTypes::SPEED:
		return AddSpeed;
	case EEffectTypes::ADD_TO_WEAPON:
		return EffAddEffectsToWeapon;
	case EEffectTypes::ADD_PROJECTILE:
		return AddProjectile;
	case EEffectTypes::ADD_MAGAZINE_SIZE:
		return AddMagazine;
	case EEffectTypes::ADD_COINS:
		return AddCoins;
	case EEffectTypes::WEAPON_PISTOL:
	case EEffectTypes::WEAPON_RIFLE:
	case EEffectTypes::WEAPON_MACHINEGUN:
	case EEffectTypes::WEAPON_LINE_REPULS:
	case EEffectTypes::WEAPON_CIRC_REPULS:
	case EEffectTypes::WEAPON_BREAD:
	case EEffectTypes::WEAPON_BURNT_BREAD:
	case EEffectTypes::WEAPON_ROCKET:
	case EEffectTypes::WEAPON_JUMPING:
	case EEffectTypes::WEAPON_MELEE:
	case EEffectTypes::WEAPON_ACID:
		return AddWeapon;
	default:
		return nullptr;
	}
}
void (* UEffect::GetOnTickFunc(UEffectsMeta* meta))(UEffect* CurEffect, float deltaTime, AActor* owner, AActor* other){
	switch (meta->effectType)
	{
	case EEffectTypes::PERIODIC_DAMAGE:
		return PeriodicalRemoveHealth;
	case EEffectTypes::HEAL:
		return PeriodicalAddHealth;
	default:
		return nullptr;
	}
}
void (* UEffect::GetOnEndFunc(UEffectsMeta* meta))(UEffect* CurEffect, AActor* owner, AActor* other){
	switch (meta->effectType)
	{
	case EEffectTypes::PARALISYS:
		return RemoveParalisys;
	case EEffectTypes::SPEED:
		return RemoveSpeed;
	case EEffectTypes::ADD_EFFECTS:
		return RemoveEffectFromUser;
	default:
		return nullptr;
	}
}
void(* UEffect::GetOnCancelFunc(UEffectsMeta* meta))(UEffect* CurEffect, AActor* owner, AActor* other){//////////////////////////////////////
	switch (meta->effectType)
	{
	case EEffectTypes::ADD_TO_WEAPON:
		return RemoveEffectFromUser;
	case EEffectTypes::PARALISYS:
		return RemoveParalisys;
	case EEffectTypes::ADD_PROJECTILE:
		return RemoveProjectile;
	case EEffectTypes::ADD_MAGAZINE_SIZE:
		return RemoveMagazine;
	case EEffectTypes::MAX_HELTH:
		return RemoveMaxHealth;
	case EEffectTypes::PROTECTION:
		return RemoveProtection;
	case EEffectTypes::SPEED:
		return &(UEffect::RemoveSpeed);
	default:
		return nullptr;
	}
}
void(* UEffect::GetBuffEffectFunc(UEffectsMeta* meta))(UEffect*, UEffect*, AActor*) {
	switch (meta->effectType)
	{
	case EEffectTypes::UP_ALL_EFFECTS:
	case EEffectTypes::UP_MOMENTAL_DAMAGE:
	case EEffectTypes::UP_PERIODIC_DAMAGE:
	case EEffectTypes::UP_HEAL:
	case EEffectTypes::UP_PARALISYS:
		return BuffEffect;
	default:
		return nullptr;
	}
}
void (* UEffect::GetDeBuffEffectFunc(UEffectsMeta* meta))(UEffect* CurEffect, UEffect*, AActor*) {
	switch (meta->effectType)
	{
	case EEffectTypes::UP_ALL_EFFECTS:
	case EEffectTypes::UP_MOMENTAL_DAMAGE:
	case EEffectTypes::UP_PERIODIC_DAMAGE:
	case EEffectTypes::UP_HEAL:
	case EEffectTypes::UP_PARALISYS:
		return DeBuffEffect;
	default:
		return nullptr;
	}
}


TArray<EEffectTypes> UEffect::GetLinkedEffects(UEffectsMeta* meta){
	switch (meta->effectType)
	{
	case EEffectTypes::UP_MOMENTAL_DAMAGE:
		return TArray<EEffectTypes> {EEffectTypes::MOMENTAL_DAMAGE};
	case EEffectTypes::UP_PERIODIC_DAMAGE:
		return TArray<EEffectTypes> {EEffectTypes::PERIODIC_DAMAGE};
	case EEffectTypes::UP_HEAL:
		return TArray<EEffectTypes> {EEffectTypes::HEAL};
	case EEffectTypes::UP_PARALISYS:
		return TArray<EEffectTypes> {EEffectTypes::PARALISYS};
	default:
		return TArray<EEffectTypes>();
	}
}



void UEffect::BuffEffect(UEffect* CurEffect, UEffect* effect, AActor* owner){
	if (effect->effectType == EEffectTypes::UP_ALL_EFFECTS ||
		CurEffect->linkedTypes.Contains(effect->effectType)) {
		effect->power *= CurEffect->power;
		effect->radianalPower *= CurEffect->radianalPower;
		effect->radius *= CurEffect->radius;
		effect->period *= CurEffect->period;
		effect->time *= CurEffect->time;
	}
}

void UEffect::DeBuffEffect(UEffect* CurEffect, UEffect* effect, AActor* owner){
	if (effect->effectType == EEffectTypes::UP_ALL_EFFECTS ||
		CurEffect->linkedTypes.Contains(effect->effectType)) {
		effect->power /= CurEffect->power;
		effect->radianalPower /= CurEffect->radianalPower;
		effect->radius /= CurEffect->radius;
		effect->period /= CurEffect->period;
		effect->time /= CurEffect->time;
	}
}



void UEffect::AddEffectToUser(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectUser* user = Cast<IEffectUser>(owner);
	if (user) {
		for (int i = 0; i < CurEffect->linkedEffects.Num(); i++)
			CurEffect->linkedIndexes.Add(user->Execute_ApplyEffect(owner, CurEffect->linkedEffects[i], 0));
	}
}
void UEffect::RemoveEffectFromUser(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectUser* user = Cast<IEffectUser>(owner);
	if (user) {
		for (int i = 0; i < CurEffect->linkedIndexes.Num(); i++)
			user->Execute_CancelEffectByIndex(owner, CurEffect->linkedIndexes[i]);
	}
}



void UEffect::AddSpeed(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeSpeed(owner, CurEffect->power);
	}
}
void UEffect::RemoveSpeed(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeSpeed(owner, 1.0 / CurEffect->power);
	}
}


void UEffect::AddHealth(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeHealth(owner, CurEffect->power, other);
	}
}
void UEffect::RemoveHealth(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeHealth(owner, -CurEffect->power, other);
	}
}
void UEffect::PeriodicalAddHealth(UEffect* CurEffect, float deltaTime, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		if ((CurEffect->ticks += deltaTime) > CurEffect->period) {
			CurEffect->ticks = 0;
			ownr->Execute_ChangeHealth(owner, CurEffect->power, other);
		}
	}
}
void UEffect::PeriodicalRemoveHealth(UEffect* CurEffect, float deltaTime, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		if ((CurEffect->ticks += deltaTime) > CurEffect->period) {
			CurEffect->ticks = 0;
			ownr->Execute_ChangeHealth(owner, -CurEffect->power, other);
		}
	}
}

void UEffect::AddMaxHealth(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeMaxHealth(owner, CurEffect->power, other);
	}
}
void UEffect::RemoveMaxHealth(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeMaxHealth(owner, -CurEffect->power, other);
	}
}
void UEffect::AddProtection(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeProtection(owner, CurEffect->power);
	}
}
void UEffect::RemoveProtection(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_ChangeProtection(owner, 1./CurEffect->power);
	}
}

void UEffect::AddWeapon(UEffect* CurEffect, AActor* owner, AActor* other) {
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		switch (CurEffect->effectType)
		{
		case EEffectTypes::WEAPON_RIFLE:
			ownr->Execute_AddWeapon(owner, "RIFLE");
			break;
		case EEffectTypes::WEAPON_PISTOL:
			ownr->Execute_AddWeapon(owner, "PISTOL");
			break;
		case EEffectTypes::WEAPON_MACHINEGUN:
			ownr->Execute_AddWeapon(owner, "MACHINE_GUN");
			break;
		case EEffectTypes::WEAPON_LINE_REPULS:
			ownr->Execute_AddWeapon(owner, "LINE_REPULS");
			break;
		case EEffectTypes::WEAPON_CIRC_REPULS:
			ownr->Execute_AddWeapon(owner, "CIRC_REPULS");
			break;
		case EEffectTypes::WEAPON_BREAD:
			ownr->Execute_AddWeapon(owner, "BREAD_LAUNCHER");
			break;
		case EEffectTypes::WEAPON_BURNT_BREAD:
			ownr->Execute_AddWeapon(owner, "BURNT_BREAD");
			break;		
		case EEffectTypes::WEAPON_ROCKET:
			ownr->Execute_AddWeapon(owner, "ROCKET");
			break;
		case EEffectTypes::WEAPON_JUMPING:
			ownr->Execute_AddWeapon(owner, "JUMPING");
			break;
		case EEffectTypes::WEAPON_MELEE:
			ownr->Execute_AddWeapon(owner, "MELEE");
			break;
		case EEffectTypes::WEAPON_ACID:
			ownr->Execute_AddWeapon(owner, "ACID");
			break;
		}
		
	}
}
void UEffect::EffAddEffectsToWeapon(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectUser* user = Cast<IEffectUser>(owner);
	if (user) {
		CurEffect->linkedIndexes = user->Execute_AddEffectsToWeapon(owner, CurEffect->linkedEffects);
	}
}

void UEffect::AddProjectile(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_AddProjectile(owner, CurEffect->power);
	}
}
void UEffect::RemoveProjectile(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_AddProjectile(owner, -CurEffect->power);
	}
}
void UEffect::AddMagazine(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_AddMagazineSize(owner, CurEffect->power);
	}
}
void UEffect::RemoveMagazine(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_AddMagazineSize(owner, -CurEffect->power);
	}
}

void UEffect::AddParalisys(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_SetParalisys(owner, true);
	}
}

void UEffect::RemoveParalisys(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_SetParalisys(owner, false);
	}
}

void UEffect::Repuls(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectInfluenced* ownr = Cast<IEffectInfluenced>(owner);
	if (ownr) {
		ownr->Execute_Repuls(owner, CurEffect->power, owner->GetActorLocation() - other->GetActorLocation());
	}
}

void UEffect::AddCoins(UEffect* CurEffect, AActor* owner, AActor* other){
	IEffectUser* user = Cast<IEffectUser>(owner);
	if (user) {
		user->Execute_AddCoins(owner, CurEffect->power);
	}
}
