// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaWeapon.h"

#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/LineBatchComponent.h"
#include "HumansVsAppliances/Core/ArenaPlayerController.h"
#include "../Core/ArenaGameInstance.h"
#include "HumansVsAppliances/EnemiesPawn/EnemyPawn.h"
#include "HumansVsAppliances/FuncLib/ArenaCommonTypes.h"
#include "HumansVsAppliances/PlayerPawn/ArenaPlayerPawn.h"
#include "HumansVsAppliances/Projectiles/ArenaProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AArenaWeapon::AArenaWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh "));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AArenaWeapon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AArenaWeapon::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenaWeapon::Shooting()
{
	
	if (CurrentWeaponInfo.ShootingType == EShootingType::Once)
	{
		Fire();
	}
	else if (CurrentWeaponInfo.ShootingType == EShootingType::Auto || CurrentWeaponInfo.ShootingType == EShootingType::OnceAuto)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Shooting,
			this,
			&AArenaWeapon::Fire,
			CurrentWeaponInfo.RateOfFire,
			true);
	}
}

void AArenaWeapon::SetCurPlayerController(APlayerController* NewController)
{
	CurPlayerController = NewController;
}

void AArenaWeapon::PlayFireSound_BP_Implementation(USoundBase* SoundFireWeapon)
{
	// in BP
}

/** start after action "FireAction" only on server Controller */
void AArenaWeapon::SpawnProjectile(const FVector CurrentVector)
{
	if (CurPlayerController)
	{
		const FVector SpawnLocation = ShootLocation->GetComponentLocation() + GetActorForwardVector();
		const FRotator SpawnRotation = CurPlayerController->GetControlRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
	
		AArenaProjectile* MyProjectile = Cast<AArenaProjectile>(GetWorld()->SpawnActor(CurrentWeaponInfo.ProjectileSetting.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
		if (MyProjectile)
		{
			CurrentWeaponInfo.ProjectileSetting.OwnerType = OwnerType;
			MyProjectile->InitProjectile(CurrentWeaponInfo.ProjectileSetting,
										 CurrentVector,
										 CurrentWeaponInfo.Damage,
										 CurrentWeaponInfo.MaxDamageDistance,
										 CurrentWeaponInfo.MinDamageDistance);
			
			//for (int i = 0; i < EffectsToProjectiles.Num(); i++)
			MyProjectile->SetMyOwner(Execute_GetMyOwner(this));
			MyProjectile->Execute_ApplyAllEffects(MyProjectile, EffectsToProjectiles, 0);
		}
	}
}

/** implementation method from Interface IIArenaPawn */
void AArenaWeapon::Fire()
{

	if (CurCharacter)
	{
		CurCharacter->PlayAnimMontage(CurrentWeaponInfo.AnimationOfFiring.AnimCharFire);
	}

	if (CurPlayerController && CurPlayerController->GetCharacter())
	{

		PlayFireSound_BP(CurrentWeaponInfo.SoundFireWeapon);
		
		if (OwnerType == EOwnerType::Player)
		{
			CurPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

			CalculateEndLocationOfProjectile();
		}
		else
		{
			EndLocProjectile = CurPlayerController->GetCharacter()->GetActorLocation();
		}

		switch (CurrentWeaponInfo.WeaponType)
		{
			case EWeaponType::LineLaser:
				LineRepulsion();
				break;
			case EWeaponType::CircleLaser:
				CircleRepulsion();
				break;
			case EWeaponType::Jumping:
				JumpingMech();
				break;
			case EWeaponType::MeleeWeapon:
				{
					FVector DifferenceVec = CurPlayerController->GetCharacter()->GetActorLocation() + FVector(0, 0, 90) - GetActorLocation();
					const float VecLength = sqrt(pow(DifferenceVec.X, 2) + pow(DifferenceVec.Y, 2));
					if (VecLength < 100)
					{
						UGameplayStatics::ApplyDamage(CurPlayerController->GetCharacter(),
													  CurrentWeaponInfo.Damage,
													  MyOwner->GetInstigatorController(),
													  this,
													  UDamageType::StaticClass());
					}
				}
				break;
			default:
				SpawnProjectile(EndLocProjectile);
				break;
		}

		NumOfShots += 1;

		if (NumOfShots % CurrentWeaponInfo.ClipVolume == 0 && CurrentWeaponInfo.ShootingType != EShootingType::Once)
		{
			if (CurrentWeaponInfo.ShootingType == EShootingType::Auto)
			{
				// start recharge weapon
			}
			else if (CurrentWeaponInfo.ShootingType == EShootingType::OnceAuto)
			{
				AArenaPlayerPawn* MyPlayer = Cast<AArenaPlayerPawn>(CurCharacter);
				if (MyPlayer && (CurrentWeaponInfo.WeaponType == EWeaponType::BreadLauncher ||
								CurrentWeaponInfo.WeaponType == EWeaponType::BurntBreadLauncher ||
								CurrentWeaponInfo.WeaponType == EWeaponType::RocketLauncher))
				{
					MyPlayer->LockRocket();
				}
				Destroy();
			}
		}
	}

	
	
}


/** calculating projectile's "end location"  */
void AArenaWeapon::CalculateEndLocationOfProjectile()
{
	
	FHitResult HitTrace;
	FCollisionQueryParams Params;
	FCollisionResponseParams ResponseParam;
	Params.bReturnPhysicalMaterial = true;
				
	GetWorld()->LineTraceSingleByChannel(HitTrace,
										 WorldLocation,
										 WorldLocation + WorldDirection*10000,
										 ECC_PhysicsBody,
										 Params,
										 ResponseParam);

	if (HitTrace.GetActor() == this)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Be careful you shoot yourself!"));
	}
	else if (HitTrace.GetActor())
	{
		EndLocProjectile = HitTrace.ImpactPoint;
	}
	else
	{
		EndLocProjectile = WorldLocation + WorldDirection*10000;
	}

	EndLocProjectile = EndLocProjectile * FVector(1, 1, 0) + FVector(0, 0, ShootLocation->GetComponentLocation().Z);

	if (FVector::Dist(EndLocProjectile, GetActorLocation()) < 200)
	{
		EndLocProjectile = ShootLocation->GetComponentLocation() + GetActorForwardVector() * 10000;
	}
	AArenaPlayerController* player = Cast<AArenaPlayerController>(Execute_GetMyOwner(this));
	if (player && player->SurfacePlatform) {
		EndLocProjectile.Z = player->SurfacePlatform->GetActorLocation().Z;
	}
}


void AArenaWeapon::CircleRepulsion()
{
	/*for (TObjectIterator<AEnemyPawn> Itr; Itr; ++Itr)
	{
		if (*Itr && Itr->GetDistanceTo(this) < CurrentWeaponInfo.TargetFloat_1)
		{
			UArenaCommonTypes::RepulseEnemy(*Itr, CurrentWeaponInfo.TargetVector_1);
			UGameplayStatics::ApplyDamage(*Itr,
													  CurrentWeaponInfo.Damage,
													  MyOwner->GetInstigatorController(),
													  this,
													  UDamageType::StaticClass());
		}
	}*/
	SpawnEffect_BP(CurrentWeaponInfo.EffectFireWeapon);

	TArray<AActor*> IgnoredActor { Execute_GetMyOwner(this) };
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		1,
		1,
		GetActorLocation(),
		0,
		Execute_MaxEffectRadius(this),
		5,
		nullptr, IgnoredActor, this, nullptr);


	if (AArenaPlayerPawn* MyPlayer = Cast<AArenaPlayerPawn>(CurCharacter))
	{
		MyPlayer->LockLaser();
	}
}


void AArenaWeapon::JumpingMech()
{
	if (CurCharacter)
	{
		CurCharacter->MaxJumpDamageDistance = CurrentWeaponInfo.MaxDamageDistance;
		CurCharacter->MinJumpDamageDistance = CurrentWeaponInfo.MinDamageDistance;
		CurCharacter->LaunchCharacter(GetVelocity() + FVector(0, 0,
									  CurrentWeaponInfo.TargetFloat_1), true, true);
		
		/*GetWorldTimerManager().SetTimer(TimerHandle_ActivateJumpDamage,
											this,
											&AArenaWeapon::ActivateJumpDamage,
											JumpingTime,
											false);*/
		CurCharacter->bIsJumping = true;
		CurCharacter->bReadyJumpingDamage = true;
	}

	if (AArenaPlayerPawn* MyPlayer = Cast<AArenaPlayerPawn>(CurCharacter))
	{
		MyPlayer->LockJump();
	}
	
}

void AArenaWeapon::ActivateJumpDamage()
{
	//CurCharacter->bIsJumping = true;
	//GetWorldTimerManager().ClearTimer(TimerHandle_ActivateJumpDamage);
}

void AArenaWeapon::SpawnEffect_BP_Implementation(UParticleSystem* CurFX)
{
	// in BP
}


void AArenaWeapon::LineRepulsion()
{
	TArray<struct FHitResult> OutHits;
	FCollisionObjectQueryParams ObjectList;
	ObjectList.AddObjectTypesToQuery(ECC_Pawn);
	ObjectList.AddObjectTypesToQuery(ECC_PhysicsBody);
	const FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("TraceParams")), false, this);

	const FVector Start = ShootLocation->GetComponentLocation() + GetActorForwardVector();
	const FVector End = EndLocProjectile;


	GetWorld()->LineBatcher->DrawLine(Start, End, FLinearColor::Red, 10, 10.0f, 30);
	
			
	GetWorld()->LineTraceMultiByObjectType(OutHits, Start, End, ObjectList, TraceParams);

	for (auto& One: OutHits)
	{
		if (AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(One.GetActor()))
		{
			//UArenaCommonTypes::RepulseEnemy(EnemyPawn, CurrentWeaponInfo.TargetVector_1);
			UGameplayStatics::ApplyDamage(EnemyPawn,
											CurrentWeaponInfo.Damage,
											MyOwner->GetInstigatorController(),
											this,
											UDamageType::StaticClass());
		}
	}

	
	if (AArenaPlayerPawn* MyPlayer = Cast<AArenaPlayerPawn>(CurCharacter))
	{
		MyPlayer->LockLaser();
	}
}


void AArenaWeapon::Destroyed()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Shooting);
	
	Super::Destroyed();
}


/////////////////////////////
////////	Effect user
////////////////////////////

void AArenaWeapon::SetMyOwner(AActor* myowner) {
	MyOwner = myowner;
	Cast<AArenaPlayerPawn>(MyOwner) ? OwnerType = EOwnerType::Player : OwnerType = EOwnerType::Enemy;
	if (!bVasStarted) {
		bVasStarted = true;
		Execute_StartEffectLoading(this);
	}
	CurCharacter = Cast<AArenaCharacter>(MyOwner);
	if (IEffectUser* user = Cast<AArenaPlayerPawn>(MyOwner)) {
		TArray<UEffect*> eff = user->Execute_GetEffectsToWeapon(MyOwner);
		Execute_ApplyAllEffects(this, eff, 0);
	}
}
AActor* AArenaWeapon::GetMyOwner_Implementation() {
	return MyOwner;
}


void AArenaWeapon::LoadEffect_Implementation(UEffectsMeta* effect) {
	UEffect* o = NewObject<UEffect>();
	o->Init(effect);
	Execute_ApplyEffect(this, o, 0);
}
void AArenaWeapon::LoadAllEffects_Implementation(TArray<UEffectsMeta*>& effects) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_LoadEffect(this, effects[i]);
}
int AArenaWeapon::ApplyEffect_Implementation(UEffect* effect, int toIndex) {
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
		if (Cast<AEnemyPawn>(Execute_GetMyOwner(this))) {
			if (eff->effectAim == EEffectAim::ENEMY)
				eff->effectAim = EEffectAim::PLAYER;
			else if (eff->effectAim == EEffectAim::PLAYER)
				eff->effectAim = EEffectAim::ENEMY;
		}
		for (int i = 0; i < EffectsToBuffEffects.Num(); i++)
			EffectsToBuffEffects[i]->CallBuffEffect(eff, this);
		EffectsToProjectiles.Add(eff);
		break;
	default:
		break;
	}
	return eff->index;
}
void AArenaWeapon::ApplyAllEffects_Implementation(TArray<UEffect*>& effects, int toIndex) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_ApplyEffect(this, effects[i], toIndex);
}
void AArenaWeapon::CancelEffectByIndex_Implementation(int index) {
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
TArray<int> AArenaWeapon::AddEffectsToWeapon_Implementation(const TArray<UEffect*>& effects) {
	for (int i = 0; i < effects.Num(); i++) {
		UEffect* eff = NewObject<UEffect>();
		eff->Copy(effects[i]);
		EffectsToProjectiles.Add(eff);
	}
	return TArray<int>();
}


TArray<UEffect*> AArenaWeapon::GetAllEffectsOnDistantion_Implementation(float distantion, AActor* who) {
	return FilterEffects(GetAllEffectsOnDistantion(distantion, EffectsToProjectiles), who);
}
TArray<UEffect*> AArenaWeapon::GetAllEffectsOnPoint_Implementation(AActor* who) {
	return FilterEffects(GetAllEffectsOnPoint(EffectsToProjectiles), who);
}
TArray<UEffect*> AArenaWeapon::GetAllEffects_Implementation(AActor* who) {
	return FilterEffects(EffectsToProjectiles, who);
}
float AArenaWeapon::MaxEffectRadius_Implementation() {
	return GetMaxEffectRadius(EffectsToProjectiles);
}
bool AArenaWeapon::IsPointDamage_Implementation(){
	switch (CurrentWeaponInfo.WeaponType)
	{
	case EWeaponType::CircleLaser:
	case EWeaponType::Jumping:
		return false;
	default:
		return true;
	}
}

TArray<UEffect*> AArenaWeapon::GetEffectsToWeapon_Implementation() {
	return TArray<UEffect*>();
}

void AArenaWeapon::StartEffectLoading_Implementation() {
	UArenaGameInstance* inst = Cast<UArenaGameInstance>(GetGameInstance());
	if (inst) {
		TArray<UEffectsMeta*> metas = inst->GetEffectsToApply(GetClass());
		Execute_LoadAllEffects(this, metas);
	}
}




/////////////////////////////
////////	Effect Influenced
////////////////////////////

void AArenaWeapon::AddAllEffects_Implementation(TArray<UEffect*>& effects, AActor* other, float radius) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_AddEffect(this, effects[i], other, radius);
}
void AArenaWeapon::AddEffect_Implementation(UEffect* effect, AActor* other, float radius) {
	Execute_AddEffectComponent(this, CreateComponentFromEffect(effect, this, other, radius));
}
void AArenaWeapon::AddEffectComponent_Implementation(UEffectComponent* component) {
	component->Activate(true);
}



void AArenaWeapon::ChangeSpeed_Implementation(float speed) {
	CurrentWeaponInfo.RateOfFire /= speed;
	
	if (CurrentWeaponInfo.ShootingType == EShootingType::Auto || CurrentWeaponInfo.ShootingType == EShootingType::OnceAuto)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Shooting,
			this,
			&AArenaWeapon::Fire,
			CurrentWeaponInfo.RateOfFire,
			true);
	}
}
void AArenaWeapon::ChangeHealth_Implementation(float health, AActor* other) {}
void AArenaWeapon::ChangeMaxHealth_Implementation(float health, AActor* other) {}
void AArenaWeapon::ChangeProtection_Implementation(float RateProtect) {}

void AArenaWeapon::AddWeapon_Implementation(const FString& name) {}

void AArenaWeapon::AddProjectile_Implementation(int count){
	CurrentWeaponInfo.NumberProjectileByShot += count;
}
void AArenaWeapon::AddMagazineSize_Implementation(int count) {
	CurrentWeaponInfo.ClipVolume += count;
}

void AArenaWeapon::SetParalisys_Implementation(bool isParalisys){}

bool AArenaWeapon::IsParalisys_Implementation() {
	return false;
}
