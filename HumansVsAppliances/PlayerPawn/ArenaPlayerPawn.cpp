// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaPlayerPawn.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HumansVsAppliances/ActorComponents/ArenaHealthActorComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "HumansVsAppliances/Core/ArenaGameInstance.h"
#include "HumansVsAppliances/Core/ArenaPlayerController.h"
#include "HumansVsAppliances/EnemiesPawn/ArenaEnemyAIController.h"
#include "HumansVsAppliances/EnemiesPawn/EnemyPawn.h"
#include "HumansVsAppliances/Projectiles/ArenaProjectile.h"
#include "HumansVsAppliances/Weapons/ArenaWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AArenaPlayerPawn::AArenaPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-90, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//HealthComponent = CreateDefaultSubobject<UArenaHealthActorComponent>(TEXT("HealthComponent"));
	
	MaxAngleBetweenMovementAndCursor = 30;

	GetCharacterMovement()->MaxWalkSpeed = 500;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AArenaPlayerPawn::OnHit);
	
}

/////////////////////////////
////////	Basic Overloaded
////////////////////////////




void AArenaPlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	//UseWeapon("Pistol");
}

// Called when the game starts or when spawned
void AArenaPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	Execute_StartEffectLoading(this);

	//UseWeapon("Pistol");
}

// Called every frame
void AArenaPlayerPawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementTick();
}

// Called to bind functionality to input
void AArenaPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	this->MyPlayerInputComponent = PlayerInputComponent;
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//MyWeapons.Add("BREAD_LAUNCHER");
	SpawnWeaponsByNames(PlayerInputComponent, MyWeapons);
	/*PlayerInputComponent->BindAction("RocketLaunch",	IE_Pressed, this, &AArenaPlayerPawn::RocketLaunch);
	PlayerInputComponent->BindAction("CircleRepulsion", IE_Pressed, this, &AArenaPlayerPawn::CircleRepulsion);
	PlayerInputComponent->BindAction("LineRepulsion",	IE_Pressed, this, &AArenaPlayerPawn::LineRepulsion);
	PlayerInputComponent->BindAction("Jumping",			IE_Pressed, this, &AArenaPlayerPawn::Jumping);
	
<<<<<<< HEAD
	PlayerInputComponent->BindAction("Pistol", IE_Pressed, this, &AArenaPlayerPawn::UsePistol);
	PlayerInputComponent->BindAction("MachineGun", IE_Pressed, this, &AArenaPlayerPawn::UseMachineGun);
	PlayerInputComponent->BindAction("Rifle", IE_Pressed, this, &AArenaPlayerPawn::UseRifle);
	PlayerInputComponent->BindAction("BreadLauncher", IE_Pressed, this, &AArenaPlayerPawn::BreadLauncher);
	PlayerInputComponent->BindAction("BurntBreadLauncher", IE_Pressed, this, &AArenaPlayerPawn::BurntBreadLauncher);
	*/
	
	PlayerInputComponent->BindAxis("MoveForward", this, &AArenaPlayerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AArenaPlayerPawn::MoveRight);
	
}
void AArenaPlayerPawn::SpawnWeaponsByNames(UInputComponent* PlayerInputComponent, TArray<FString> wpns){
	for (int i = 0; i < wpns.Num(); i++)
		if (wpns[i] == "PISTOL") {
			//PlayerInputComponent->BindAction("Pistol", IE_Pressed, this, &AArenaPlayerPawn::UsePistol);
			UseWeapon("Pistol");
		}
		else if (wpns[i] == "RIFLE") {
			//PlayerInputComponent->BindAction("Rifle", IE_Pressed, this, &AArenaPlayerPawn::UseRifle);
			UseWeapon("Rifle");
		}
		else if (wpns[i] == "MACHINE_GUN") {
			//PlayerInputComponent->BindAction("MachineGun", IE_Pressed, this, &AArenaPlayerPawn::UseMachineGun);
			UseWeapon("MachineGun");
		}
		else if (wpns[i] == "LINE_REPULS") {
			PlayerInputComponent->BindAction("LineRepulsion", IE_Pressed, this, &AArenaPlayerPawn::LineRepulsion);
		}
		else if (wpns[i] == "CIRC_REPULS") {
			PlayerInputComponent->BindAction("LineRepulsion", IE_Pressed, this, &AArenaPlayerPawn::CircleRepulsion);
			//PlayerInputComponent->BindAction("CircleRepulsion", IE_Pressed, this, &AArenaPlayerPawn::CircleRepulsion);
		}
		else if (wpns[i] == "BREAD_LAUNCHER") {
			PlayerInputComponent->BindAction("RocketLaunch", IE_Pressed, this, &AArenaPlayerPawn::BreadLauncher);
			//PlayerInputComponent->BindAction("BreadLauncher", IE_Pressed, this, &AArenaPlayerPawn::BreadLauncher);
		}
		else if (wpns[i] == "BURNT_BREAD") {
			PlayerInputComponent->BindAction("RocketLaunch", IE_Pressed, this, &AArenaPlayerPawn::BurntBreadLauncher);
			//PlayerInputComponent->BindAction("BurntBreadLauncher", IE_Pressed, this, &AArenaPlayerPawn::BurntBreadLauncher);
		}
		else if (wpns[i] == "ROCKET") {
			PlayerInputComponent->BindAction("RocketLaunch", IE_Pressed, this, &AArenaPlayerPawn::RocketLaunch);
		}
		else if (wpns[i] == "JUMPING") {
			PlayerInputComponent->BindAction("Jumping", IE_Pressed, this, &AArenaPlayerPawn::Jumping);
		}
}

float AArenaPlayerPawn::TakeDamage(const float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                   AActor* DamageCauser)
{
	IEffectUser* other = Cast<IEffectUser>(DamageCauser);
	/*if (DamageEvent.DamageTypeClass == UDamageType::StaticClass()) {
	}*/
	if (other) {
		const bool bIsPoint = other->Execute_IsPointDamage(DamageCauser);
		TArray<UEffect*> Eff = bIsPoint ?
			other->Execute_GetAllEffectsOnPoint(DamageCauser, this) :
			other->Execute_GetAllEffectsOnDistantion(DamageCauser, (DamageCauser->GetActorLocation() - GetActorLocation()).Size(), this);
		Execute_AddAllEffects(this, Eff, other->Execute_GetMyOwner(DamageCauser), bIsPoint ? 0 : (DamageCauser->GetActorLocation() - GetActorLocation()).Size());
	}

	//HealthComponent->ChangeHealthValue(-Damage);
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

//void AArenaPlayerPawn::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//	if (bIsJumping)
//	{
//		bIsJumping = false;
//		DrawDebugSphere(GetWorld(), GetActorLocation(), MaxJumpDamage, 8, FColor::Red, false, 4.0f);
//		DrawDebugSphere(GetWorld(), GetActorLocation(), MinJumpDamage, 8, FColor::Blue, false, 4.0f);
//
//		TArray<AActor*> IgnoredActor;
//		IgnoredActor.Add(this);
//		UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
//				1000,
//				1000*0.2f,
//				GetActorLocation(),
//				MaxJumpDamage,
//				MinJumpDamage,
//				5,
//				NULL, IgnoredActor,this,nullptr);
//	}
//}

/////////////////////////////
////////	Movement
////////////////////////////

/** MoveForward for character  */
void AArenaPlayerPawn::MoveForward(const float Value)
{
	AxisX = Value;
}

/** MoveRight for character  */
void AArenaPlayerPawn::MoveRight(const float Value)
{
	AxisY = Value;
}

/**	if cursor change location character will be change rotation to cursor	*/
void AArenaPlayerPawn::CalculateNewCharacterRotation()
{
	if (IsCanRotate)
	{
		// get vector to cursor
		FVector2D Pos = FVector2D::ZeroVector;
		const UGameViewportClient* MyViewport = GetWorld()->GetGameViewport();
		MyViewport->GetMousePosition(Pos);
		const FVector VectorToCursor = FVector(Pos.X, Pos.Y, 0);
		
		// get center of viewport display
		const FVector2D ViewportSize = FVector2D(MyViewport->Viewport->GetSizeXY());
		const FVector2D  ViewportCenter =  FVector2D(ViewportSize.X/2, ViewportSize.Y/2);
		const FVector VectorToCenter = UKismetMathLibrary::Conv_Vector2DToVector(ViewportCenter);
		
		// get yaw to cursor
		YawToCursor = UKismetMathLibrary::FindLookAtRotation(VectorToCenter, VectorToCursor).Yaw;
	}
}

/** tick of character movement  */
void AArenaPlayerPawn::MovementTick()
{
	if ((Controller != nullptr))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector XDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(XDirection, AxisX);

		const FVector YDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(YDirection, AxisY);

		// rotate character to cursor position
		CalculateNewCharacterRotation();
		SetActorRotation(FQuat(FRotator(0.0f, YawToCursor + 90, 0.0f)));
	}
}

/////////////////////////////
////////	Attack
////////////////////////////

void AArenaPlayerPawn::UseWeapon(FName NameWeapon, bool IsOnce)
{
	if (AArenaPlayerController* CurrentController = Cast<AArenaPlayerController>(GetController()))
	{
		if (CurrentWeapon && !IsOnce)
			CurrentWeapon->Destroy();
		
		AArenaWeapon* NewWeapon = UArenaCommonTypes::SetWeapon(GetWorld(), GetMesh(), this,
													 GetInstigator(), CurrentController, NameWeapon);
		NewWeapon->Shooting();

		if (NewWeapon && NameWeapon == "Jumping")
			jumpingWeapon = NewWeapon;
		if (NewWeapon && !IsOnce)
			CurrentWeapon = NewWeapon;
	}
}

void AArenaPlayerPawn::RocketLaunch()
{
	if (IsActiveRocket)
	{
		IsActiveRocket = false;
		UseWeapon("RocketLauncher", true);
	}
}

void AArenaPlayerPawn::UsePistol()
{
	UseWeapon("Pistol");
}

void AArenaPlayerPawn::UseRifle()
{
	UseWeapon("Rifle");
}

void AArenaPlayerPawn::UseMachineGun()
{
	UseWeapon("MachineGun");
}

void AArenaPlayerPawn::BreadLauncher()
{
	if (IsActiveRocket)
	{
		IsActiveRocket = false;
		UseWeapon("BreadLauncher", true);
	}
}

void AArenaPlayerPawn::BurntBreadLauncher()
{
	if (IsActiveRocket)
	{
		IsActiveRocket = false;
		UseWeapon("BurntBreadLauncher", true);
	}
}

void AArenaPlayerPawn::Jumping()
{
	if (IsActiveJump)
	{
		IsActiveJump = false;
		UseWeapon("Jumping", true);
	}
}

void AArenaPlayerPawn::LineRepulsion()
{
	if (IsActiveLaser)
	{
		IsActiveLaser = false;
		UseWeapon("LineLaser", true);
	}
}

void AArenaPlayerPawn::CircleRepulsion()
{
	if (IsActiveLaser)
	{
		IsActiveLaser = false;
		UseWeapon("CircleLaser", true);
	}
}



void AArenaPlayerPawn::LockRocket()
{
	OnChangeStateRocket.Broadcast(false);
	GetWorldTimerManager().SetTimer(TimerHandle_ActivateRocket,
									this,
									&AArenaPlayerPawn::ActivateRocket,
									TimeToActiveRocket,
									true);
}

void AArenaPlayerPawn::ActivateRocket()
{
	IsActiveRocket = true;
	OnChangeStateRocket.Broadcast(true);
	GetWorldTimerManager().ClearTimer(TimerHandle_ActivateRocket);
}

void AArenaPlayerPawn::LockLaser()
{
	OnChangeStateLaser.Broadcast(false);
	GetWorldTimerManager().SetTimer(TimerHandle_ActivateLaser,
									this,
									&AArenaPlayerPawn::ActivateLaser,
									TimeToActiveLaser,
									true);
}

void AArenaPlayerPawn::ActivateLaser()
{
	IsActiveLaser = true;
	OnChangeStateLaser.Broadcast(true);
	GetWorldTimerManager().ClearTimer(TimerHandle_ActivateLaser);
}

void AArenaPlayerPawn::LockJump()
{
	OnChangeStateJump.Broadcast(false);
	GetWorldTimerManager().SetTimer(TimerHandle_ActivateJump,
									this,
									&AArenaPlayerPawn::ActivateJump,
									TimeToActiveJump,
									true);
}

void AArenaPlayerPawn::ActivateJump()
{
	IsActiveJump = true;
	OnChangeStateJump.Broadcast(true);
	GetWorldTimerManager().ClearTimer(TimerHandle_ActivateJump);
}

/////////////////////////////
////////	Mechanics
////////////////////////////

/*void AArenaPlayerPawn::ActivateJumpDamage()
{
	bIsJumping = true;
	GetWorldTimerManager().ClearTimer(TimerHandle_ActivateJumpDamage);
}*/


AActor* AArenaPlayerPawn::GetJumpingWeapon(){
	return jumpingWeapon;
}
float AArenaPlayerPawn::GetJumpingDamageRadius(){
	return jumpingWeapon ? jumpingWeapon->Execute_MaxEffectRadius(jumpingWeapon) : 0;
}


/////////////////////////////
////////	Effect user
////////////////////////////

AActor* AArenaPlayerPawn::GetMyOwner_Implementation() {
	return this;
}

void AArenaPlayerPawn::LoadEffect_Implementation(UEffectsMeta* effect) {
	UEffect* o = NewObject<UEffect>();
	o->Init(effect);
	Execute_ApplyEffect(this, o, 0);
}
void AArenaPlayerPawn::LoadAllEffects_Implementation(TArray<UEffectsMeta*>& effects) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_LoadEffect(this, effects[i]);
}
int AArenaPlayerPawn::ApplyEffect_Implementation(UEffect* effect, int toIndex) {

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
	return -1;
}
void AArenaPlayerPawn::ApplyAllEffects_Implementation(TArray<UEffect*>& effects, int toIndex) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_ApplyEffect(this, effects[i], toIndex);
}
void AArenaPlayerPawn::CancelEffectByIndex_Implementation(int index){
	for (int i = OtherEffects.Num() - 1; i >= 0; i--)
		if (OtherEffects[i]->index == index) {
			OtherEffects[i]->CallCancel(this, this);
			OtherEffects.RemoveAt(i);
		}
	for (int i = WeaponEffects.Num() - 1; i >= 0; i--)
		if (WeaponEffects[i]->index == index) {
			if (CurrentWeapon) {
				//for (int j = 0; j < WeaponEffects[i]->linkedIndexes.Num(); j++)
				CurrentWeapon->Execute_CancelEffectByIndex(CurrentWeapon, -WeaponEffects[i]->index);
			}

			WeaponEffects.RemoveAt(i);
		}
}
TArray<int> AArenaPlayerPawn::AddEffectsToWeapon_Implementation(const TArray<UEffect*>& effects) {
	TArray<UEffect*> effs = effects;
	TArray<int> res;
	
	for (int i = 0; i < effs.Num(); i++) {
		UEffect* e = NewObject<UEffect>();
		e->Copy(effs[i]);
		res.Add(e->index = EffectIndex++);
		WeaponEffects.Add(e);
		if (CurrentWeapon) {
			CurrentWeapon->Execute_ApplyEffect(CurrentWeapon, e, -e->index);
		}
	}
	return res;
}


TArray<UEffect*> AArenaPlayerPawn::GetAllEffectsOnDistantion_Implementation(float distantion, AActor* who) {
	TArray<UEffect*> res;
	return res;
}
TArray<UEffect*> AArenaPlayerPawn::GetAllEffectsOnPoint_Implementation(AActor* who) {
	TArray<UEffect*> res;
	return res;
}
TArray<UEffect*> AArenaPlayerPawn::GetAllEffects_Implementation(AActor* who) {
	TArray<UEffect*> res;
	return res;
}
float AArenaPlayerPawn::MaxEffectRadius_Implementation() {
	return 0;
}
bool AArenaPlayerPawn::IsPointDamage_Implementation(){
	return false;
}


TArray<UEffect*> AArenaPlayerPawn::GetEffectsToWeapon_Implementation(){
	return WeaponEffects;
}

void AArenaPlayerPawn::AddCoins_Implementation(int count) {
	if (UArenaGameInstance* gi = Cast<UArenaGameInstance>(GetGameInstance())) {
		gi->Coins += count;
	}
}


void AArenaPlayerPawn::StartEffectLoading_Implementation() {
	UArenaGameInstance* inst = Cast<UArenaGameInstance>(GetGameInstance());
	if (inst) {
		TArray<UEffectsMeta*> metas = inst->GetEffectsToApply(GetClass());
		Execute_LoadAllEffects(this, metas);
	}
}


/////////////////////////////
////////	Effect Influenced
////////////////////////////

void AArenaPlayerPawn::AddAllEffects_Implementation(TArray<UEffect*>& effects, AActor* other, float radius) {
	for (int i = 0; i < effects.Num(); i++)
		Execute_AddEffect(this, effects[i], other, radius);
}
void AArenaPlayerPawn::AddEffect_Implementation(UEffect* effect, AActor* other, float radius) {
	Execute_AddEffectComponent(this, CreateComponentFromEffect(effect, this, other, radius));
}
void AArenaPlayerPawn::AddEffectFromMeta_Implementation(UEffectsMeta* effect, AActor* other, float radius) {
	UEffect* o = NewObject<UEffect>();
	o->Init(effect);
	Execute_AddEffect(this, o, other, radius);
}
void AArenaPlayerPawn::AddEffectComponent_Implementation(UEffectComponent* component) {
	component->Activate(true);
}




void AArenaPlayerPawn::ChangeSpeed_Implementation(float speed){
	GetCharacterMovement()->MaxWalkSpeed *= speed;
}
void AArenaPlayerPawn::ChangeHealth_Implementation(float health, AActor* other){
	if (!(health < 0 && bIsJumping)) {
		HealthComponent->ChangeHealthValue(health);
		if (health < 0)
			StartTakeDamageAnimation();
		AArenaPlayerController* pc = Cast<AArenaPlayerController>(Controller);
		if (pc)
			pc->AddDamageToViewport(this, health);
	}
}
void AArenaPlayerPawn::ChangeMaxHealth_Implementation(float health, AActor* other) {
	HealthComponent->MaxHealth *= health;
}
void AArenaPlayerPawn::ChangeProtection_Implementation(float RateProtect){
	HealthComponent->RateDamage /= RateProtect;
}

void AArenaPlayerPawn::AddWeapon_Implementation(const FString& name){
	MyWeapons.Add(name);
	if (MyPlayerInputComponent) {
		SpawnWeaponsByNames(MyPlayerInputComponent, TArray<FString>{name});
	}
}
void AArenaPlayerPawn::AddProjectile_Implementation(int count) {}
void AArenaPlayerPawn::AddMagazineSize_Implementation(int count) {}

void AArenaPlayerPawn::SetParalisys_Implementation(bool isParalisys) {
	this->countParalisys += isParalisys ? 1 : -1;
}
bool AArenaPlayerPawn::IsParalisys_Implementation() {
	return this->countParalisys > 0;
}

void AArenaPlayerPawn::Repuls_Implementation(float power, FVector dir) {
	//UArenaCommonTypes::RepulseEnemy(this, dir.Normalize() * FVector(power));
}