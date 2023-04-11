// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"

#include "../ActorComponents/ArenaHealthActorComponent.h"
#include "../Core/ArenaPlayerController.h"
#include "../PlayerPawn/ArenaPlayerPawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HumansVsAppliances/Projectiles/ArenaProjectile.h"
#include "HumansVsAppliances/Weapons/ArenaWeapon.h"
#include "../Core/ArenaGameInstance.h"
#include "Kismet/GameplayStatics.h"




// Sets default values
AEnemyPawn::AEnemyPawn()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/////////////////////////////
////////	Basic Overloaded
////////////////////////////

void AEnemyPawn::PossessedBy(AController* NewController)
{
	PlController = GetWorld()->GetFirstPlayerController();
	PlChar = PlController->GetPawn();
	Super::PossessedBy(NewController);
}

void AEnemyPawn::StartTakeDamageAnimation()
{
	if (bIsAlive)
	{
		PlayDamageSound_BP();
		LastSpeed = GetCharacterMovement()->MaxWalkSpeed;

		GetCharacterMovement()->MaxWalkSpeed = 0;
		GetWorldTimerManager().SetTimer(TimerHandle_PlayingDamage,
									this,
									&AEnemyPawn::ReturnLastSpeed,
									PlayAnimMontage(AnimTakeDamage, 2)/4,
									true);
	}
}

void AEnemyPawn::ReturnLastSpeed()
{
	if (bIsAlive)
		GetCharacterMovement()->MaxWalkSpeed = LastSpeed;
	GetWorldTimerManager().ClearTimer(TimerHandle_PlayingDamage);
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Execute_StartEffectLoading(this);
}

// Called every frame
void AEnemyPawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentDeltaTime += DeltaTime;
}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemyPawn::TakeDamage(const float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                             AActor* DamageCauser)
{
	const IEffectUser* Other = Cast<IEffectUser>(DamageCauser);
	
	if (Other) {
		bool isPoint = Other->Execute_IsPointDamage(DamageCauser);
		TArray<UEffect*> Eff = isPoint ? 
			Other->Execute_GetAllEffectsOnPoint(DamageCauser, this) :
			Other->Execute_GetAllEffectsOnDistantion(DamageCauser, (DamageCauser->GetActorLocation() - GetActorLocation()).Size(), this);
		Execute_AddAllEffects(this, Eff, Other->Execute_GetMyOwner(DamageCauser), isPoint ? 0 : (DamageCauser->GetActorLocation() - GetActorLocation()).Size());
	}
	
	//HealthComponent->ChangeHealthValue(-Damage);
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AEnemyPawn::Destroyed()
{
	Super::Destroyed();
}

/////////////////////////////
////////	Attack
////////////////////////////

void AEnemyPawn::AttackImpl()
{
	if (PlChar && CurrentWeapon && CurrentDeltaTime - PastShootingTime >= CurrentWeapon->CurrentWeaponInfo.RateOfFire)
	{
		CurrentWeapon->Shooting();
		PastShootingTime = CurrentDeltaTime;
	}
}

/////////////////////////////
////////	Score
////////////////////////////

void AEnemyPawn::NotifyToController()
{
	if (AArenaPlayerController* MyController = Cast<AArenaPlayerController>(PlController))
	{
		MyController->NotifyFromEnemy(CutEnemyType);
	}
}

void AEnemyPawn::SetParams(int HealthInPercent, int MaxSpeed)
{
	HealthComponent->MaxHealth = HealthInPercent;
	HealthComponent->CurrentHealth = HealthInPercent;
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
}

/////////////////////////////
////////	Movement
////////////////////////////
//
//void AEnemyPawn::MoveToDirection(const float DeltaTime)
//{
//	const FVector DeltaLocation = NewDirection * CurrentSpeed * DeltaTime;
//	AddActorWorldOffset(DeltaLocation, true);
//}

/*void AEnemyPawn::MoveFaceToPlayer()
{
	FaceRotation(NewRotation, 100);
}*/

/////////////////////////////
////////	Health
////////////////////////////

void AEnemyPawn::CharacterDead()
{
	Super::CharacterDead();
	NotifyToController();
	if (CurrentWeapon)
		CurrentWeapon->Destroy();
	this->Destroy();
}



AActor* AEnemyPawn::GetJumpingWeapon() {
	return nullptr;
}

float AEnemyPawn::GetJumpingDamageRadius() {
	return 0;
}


/////////////////////////////
////////	Effect user
////////////////////////////

AActor* AEnemyPawn::GetMyOwner_Implementation() {
	return this;
}


void AEnemyPawn::LoadEffect_Implementation(UEffectsMeta* effect) {
	UEffect* o = NewObject<UEffect>();
	o->Init(effect);
	Execute_ApplyEffect(this, o, 0);
}
void AEnemyPawn::LoadAllEffects_Implementation(TArray<UEffectsMeta*>& effects) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_LoadEffect(this, effects[i]);
}
int AEnemyPawn::ApplyEffect_Implementation(UEffect* effect, int toIndex) {
	UEffect* eff = NewObject<UEffect>();
	switch (effect->GetTypeOfType())
	{
	case ETepesEffectType::UP_USER:
		eff->Copy(effect);
		eff->index = toIndex ? toIndex : EffectIndex++;
		eff->CallStart(this, this);
		OtherEffects.Add(eff);
		return eff->index;
	case ETepesEffectType::UP_EFFECT:
	case ETepesEffectType::INFLUENCE:
	default:
		break;
	}
	return eff->index;
}
void AEnemyPawn::ApplyAllEffects_Implementation(TArray<UEffect*>& effects, int toIndex) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_ApplyEffect(this, effects[i], toIndex);
}
void AEnemyPawn::CancelEffectByIndex_Implementation(int index) {
	for (int i = OtherEffects.Num() - 1; i >= 0; i--)
		if (OtherEffects[i]->index == index) {
			OtherEffects[i]->CallCancel(this, this);
			OtherEffects.RemoveAt(i);
		}
}
TArray<int> AEnemyPawn::AddEffectsToWeapon_Implementation(const TArray<UEffect*>& effects) {
	/*for (int i = 0; i < effects.Num(); i++) {
		UEffect* eff = NewObject<UEffect>();
		eff->Copy(effects[i]);
		EffectsToProjectiles.Add(eff);
	}*/
	return TArray<int>();
}
TArray<UEffect*> AEnemyPawn::GetEffectsToWeapon_Implementation() {
	return TArray<UEffect*>();
}


TArray<UEffect*> AEnemyPawn::GetAllEffectsOnDistantion_Implementation(float distantion, AActor* who) {
	TArray<UEffect*> res;
	return res;
}
TArray<UEffect*> AEnemyPawn::GetAllEffectsOnPoint_Implementation(AActor* who) {
	TArray<UEffect*> res;
	return res;
}

TArray<UEffect*> AEnemyPawn::GetAllEffects_Implementation(AActor* who) {
	TArray<UEffect*> res;
	return res;
}
float AEnemyPawn::MaxEffectRadius_Implementation() {
	return 0;
}
bool AEnemyPawn::IsPointDamage_Implementation() {
	return false;
}


void AEnemyPawn::StartEffectLoading_Implementation() {
	UArenaGameInstance* inst = Cast<UArenaGameInstance>(GetGameInstance());
	if (inst) {
		TArray<UEffectsMeta*> metas = inst->GetEffectsToApply(GetClass());
		Execute_LoadAllEffects(this, metas);
	}
}



/////////////////////////////
////////	Effect Influenced
////////////////////////////
void AEnemyPawn::AddAllEffects_Implementation(TArray<UEffect*>& effects, AActor* other, float radius){
	for (int i = 0; i < effects.Num(); i++)
		Execute_AddEffect(this, effects[i], other, radius);
}
void AEnemyPawn::AddEffect_Implementation(UEffect* effect, AActor* other, float radius){
	Execute_AddEffectComponent(this, CreateComponentFromEffect(effect, this, other, radius));
}
void AEnemyPawn::AddEffectComponent_Implementation(UEffectComponent* component){
	component->Activate(true);
}



void AEnemyPawn::ChangeSpeed_Implementation(float speed) {
	//GetCharacterMovement()->MaxWalkSpeed *= speed;
	//CurrentSpeed *= speed;

	GetCharacterMovement()->MaxWalkSpeed *= speed;
}
void AEnemyPawn::ChangeHealth_Implementation(float health, AActor* other) {
	if (health < 0)
		StartTakeDamageAnimation();
	HealthComponent->ChangeHealthValue(health);
	AArenaPlayerController* pc = Cast<AArenaPlayerController>(PlController);
	if (pc)
		pc->AddDamageToViewport(this, health);
}
void AEnemyPawn::ChangeMaxHealth_Implementation(float health, AActor* other) {
	HealthComponent->MaxHealth *= health;
}
void AEnemyPawn::ChangeProtection_Implementation(float RateProtect) {
	HealthComponent->RateDamage /= RateProtect;
}

void AEnemyPawn::AddWeapon_Implementation(const FString& name) {
	if (name == "MELEE")
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"MeleeWeapon");
	else if (name == "ACID") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
		"AcidWeapon");
	}
	else if (name == "PISTOL") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"Pistol");
	}
	else if (name == "RIFLE") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"Rifle");
	}
	else if (name == "MACHINE_GUN") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"MachineGun");
	}
	else if (name == "BREAD_LAUNCHER") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"BreadLauncher");
	}
	else if (name == "BURNT_BREAD") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"BurntBreadLauncher");
	}
	else if (name == "ROCKET") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"RocketLaunch");
	}
	else if (name == "JUMPING") {
		CurrentWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
			GetInstigator(), GetWorld()->GetFirstPlayerController(),
			"Jumping");
	}
	CurrentWeapon->SetMyOwner(this);
}
void AEnemyPawn::AddProjectile_Implementation(int count) {}
void AEnemyPawn::AddMagazineSize_Implementation(int count) {}

void AEnemyPawn::SetParalisys_Implementation(bool isParalisys) {
	this->countParalisys += isParalisys ? 1 : -1;
}
bool AEnemyPawn::IsParalisys_Implementation() {
	return this->countParalisys > 0;
}

void AEnemyPawn::Repuls_Implementation(float power, FVector dir){
	FVector d = dir * FVector(1, 1, 0);;
	d.Normalize();
	d = (d + FVector::UpVector) * FVector(power, power, 0.5 * power);
	UArenaCommonTypes::RepulseEnemy(this, d);
}