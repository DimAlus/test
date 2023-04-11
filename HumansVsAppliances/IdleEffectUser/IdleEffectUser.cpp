// Fill out your copyright notice in the Description page of Project Settings.


#include "IdleEffectUser.h"

#include "Kismet/GameplayStatics.h"
#include "../PlayerPawn/ArenaPlayerPawn.h"
#include "../Core/ArenaGameInstance.h"

// Sets default values
AIdleEffectUser::AIdleEffectUser()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AIdleEffectUser::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AIdleEffectUser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIdleEffectUser::OnOtherActorBeginOverlap(AActor* OtherActor) {
	if (loaded) {
		if (CurrentInfo.IdolType == EIdleEffectUserType::PICKED) {
			AArenaPlayerPawn* player = Cast<AArenaPlayerPawn>(OtherActor);
			if (player) {
				GetWorldTimerManager().ClearAllTimersForObject(this);
				UGameplayStatics::ApplyDamage(player,
					1,
					GetInstigatorController(),
					this,
					nullptr);
				Destroy();
			}
		}
	}
}
void AIdleEffectUser::Dead(){
	Destroy();
}
TArray<FEnemyDropData> AIdleEffectUser::GetEffectsToPlayer(){
	return EffectsToPlayer;
}
void AIdleEffectUser::InitWithEffects(TArray<FEnemyDropData>& AddEffects) {
	EffectsToPlayer = AddEffects;
	Init();
}
void AIdleEffectUser::Init()
{
	if (!loaded) {

		Execute_StartEffectLoading(this);
		loaded = true;
		GetWorldTimerManager().SetTimer(TimerHandle_Death,
			this,
			&AIdleEffectUser::Dead,
			CurrentInfo.TimeLife,
			false);
		if (CurrentInfo.IdolType == EIdleEffectUserType::PICKED) {
			//MyCollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &AIdleEffectUser::OnOverlapBegin);
		}
		else {
			StartFire();
		}
	}
}
void AIdleEffectUser::StartFire()
{
	GetWorldTimerManager().SetTimer(TimerHandle_Shooting,
		this,
		&AIdleEffectUser::Fire,
		CurrentInfo.RateOfFire,
		true);
}

void AIdleEffectUser::Fire()
{
	const TArray<AActor*> IgnoredActor;

	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		1,
		1,
		GetActorLocation(),
		0,
		Execute_MaxEffectRadius(this),
		5,
		nullptr, IgnoredActor, this, nullptr);

	NumOfShots += 1;
	if (NumOfShots % CurrentInfo.ClipVolume == 0) {
		GetWorldTimerManager().SetTimer(TimerHandle_Shooting,
			this,
			&AIdleEffectUser::StartFire,
			CurrentInfo.RechargeTime,
			false);
	}
}





/////////////////////////////
////////	Effect user
////////////////////////////

AActor* AIdleEffectUser::GetMyOwner_Implementation() {
	return this;
}


void AIdleEffectUser::LoadEffect_Implementation(UEffectsMeta* effect) {
	UEffect* o = NewObject<UEffect>();
	o->Init(effect);
	Execute_ApplyEffect(this, o, 0);
}
void AIdleEffectUser::LoadAllEffects_Implementation(TArray<UEffectsMeta*>& effects) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_LoadEffect(this, effects[i]);
}
int AIdleEffectUser::ApplyEffect_Implementation(UEffect* effect, int toIndex) {
	UEffect* eff = NewObject<UEffect>();
	eff->Copy(effect);
	eff->index = toIndex ? toIndex : EffectIndex++;

	switch (effect->GetTypeOfType())
	{
	case ETepesEffectType::UP_USER:
		eff->CallStart(this, this);
		OtherEffects.Add(eff);
		break;
	case ETepesEffectType::UP_EFFECT:
		for (int i = 0; i < EffectsToProjectiles.Num(); i++)
			eff->CallBuffEffect(EffectsToProjectiles[i], this);
		EffectsToBuffEffects.Add(eff);
		break;
	case ETepesEffectType::INFLUENCE:
		for (int i = 0; i < EffectsToBuffEffects.Num(); i++)
			EffectsToBuffEffects[i]->CallBuffEffect(eff, this);
		EffectsToProjectiles.Add(eff);
		break;
	default:
		break;
	}
	return eff->index;
}
void AIdleEffectUser::ApplyAllEffects_Implementation(TArray<UEffect*>& effects, int toIndex) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_ApplyEffect(this, effects[i], toIndex);
}
void AIdleEffectUser::CancelEffectByIndex_Implementation(int index) {
	for (int i = OtherEffects.Num() - 1; i >= 0; i--)
		if (OtherEffects[i]->index == index) {
			OtherEffects[i]->CallCancel(this, this);
			OtherEffects.RemoveAt(i);
			return;
		}
	for (int i = EffectsToBuffEffects.Num() - 1; i >= 0; i--)
		if (EffectsToBuffEffects[i]->index == index) {
			for (int j = 0; j < EffectsToProjectiles.Num(); j++)
				EffectsToBuffEffects[i]->CallDeBuffEffect(EffectsToProjectiles[j], this);
			EffectsToBuffEffects[i]->CallCancel(this, this);
			EffectsToBuffEffects.RemoveAt(i);
			return;
		}
	for (int i = EffectsToProjectiles.Num() - 1; i >= 0; i--)
		if (EffectsToProjectiles[i]->index == index) {
			EffectsToProjectiles[i]->CallCancel(this, this);
			EffectsToProjectiles.RemoveAt(i);
			return;
		}
}
TArray<int> AIdleEffectUser::AddEffectsToWeapon_Implementation(const TArray<UEffect*>& effects) {
	return TArray<int>();
}

TArray<UEffect*> AIdleEffectUser::GetAllEffectsOnDistantion_Implementation(float distantion, AActor* who) {
	return FilterEffects(GetAllEffectsOnDistantion(distantion, EffectsToProjectiles), who);
}
TArray<UEffect*> AIdleEffectUser::GetAllEffectsOnPoint_Implementation(AActor* who) {
	return Execute_GetAllEffects(this, who);
}
TArray<UEffect*> AIdleEffectUser::GetAllEffects_Implementation(AActor* who) {
	return FilterEffects(EffectsToProjectiles, who);
}
float AIdleEffectUser::MaxEffectRadius_Implementation() {
	return GetMaxEffectRadius(EffectsToProjectiles);
}
bool AIdleEffectUser::IsPointDamage_Implementation() {
	return CurrentInfo.IdolType == EIdleEffectUserType::PICKED;
}


TArray<UEffect*> AIdleEffectUser::GetEffectsToWeapon_Implementation() {
	return TArray<UEffect*>();
}

void AIdleEffectUser::StartEffectLoading_Implementation() {
	UArenaGameInstance* inst = Cast<UArenaGameInstance>(GetGameInstance());
	if (inst) {
		TArray<UEffectsMeta*> metas = inst->GetEffectsToApply(GetClass());
		Execute_LoadAllEffects(this, metas);
	}
}




/////////////////////////////
////////	Effect Influenced
////////////////////////////

void AIdleEffectUser::AddAllEffects_Implementation(TArray<UEffect*>& effects, AActor* other, float radius) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_AddEffect(this, effects[i], other, radius);
}
void AIdleEffectUser::AddEffect_Implementation(UEffect* effect, AActor* other, float radius) {
	Execute_AddEffectComponent(this, CreateComponentFromEffect(effect, this, other, radius));
}
void AIdleEffectUser::AddEffectComponent_Implementation(UEffectComponent* component) {
	component->Activate(true);
}



void AIdleEffectUser::ChangeSpeed_Implementation(float speed) {
	CurrentInfo.RateOfFire /= speed;

	/*if (CurrentWeaponInfo.ShootingType == EShootingType::Auto || CurrentWeaponInfo.ShootingType == EShootingType::OnceAuto)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Shooting,
			this,
			&AIdleEffectUser::Fire,
			CurrentWeaponInfo.RateOfFire,
			true);
	}*/
}
void AIdleEffectUser::ChangeHealth_Implementation(float health, AActor* other) {}
void AIdleEffectUser::ChangeMaxHealth_Implementation(float health, AActor* other) {}
void AIdleEffectUser::ChangeProtection_Implementation(float RateProtect) {}

void AIdleEffectUser::AddWeapon_Implementation(const FString& name) {}

void AIdleEffectUser::AddProjectile_Implementation(int count) {}
void AIdleEffectUser::AddMagazineSize_Implementation(int count) {
	CurrentInfo.ClipVolume += count;
}

void AIdleEffectUser::SetParalisys_Implementation(bool isParalisys) {}

bool AIdleEffectUser::IsParalisys_Implementation() {
	return false;
}
