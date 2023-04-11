// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaProjectile.h"
#include "DrawDebugHelpers.h"
#include "HumansVsAppliances/Environment/ArenaSpawnObject.h"
#include "../Core/ArenaGameInstance.h"
#include "HumansVsAppliances/PlayerPawn/ArenaPlayerPawn.h"
#include "HumansVsAppliances/Weapons/ArenaWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AArenaProjectile::AArenaProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
    }

    if (!CollisionComponent)
    {
        // Use a sphere as a simple collision representation.
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
        // Set the sphere's collision profile name to "Projectile".
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
        // Event called when component hits something.
        CollisionComponent->OnComponentHit.AddDynamic(this, &AArenaProjectile::OnHit);
        // Set the sphere's collision radius.
        CollisionComponent->InitSphereRadius(15.0f);
        // Set the root component to be the collision component.
        RootComponent = CollisionComponent;
    }

    if (!ProjectileMovementComponent)
    {
        // Use this component to drive this projectile's movement.
        ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
        ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
        ProjectileMovementComponent->InitialSpeed = 1.f;
        ProjectileMovementComponent->MaxSpeed = 0.f;
        ProjectileMovementComponent->bRotationFollowsVelocity = true;
        ProjectileMovementComponent->bShouldBounce = true;
        ProjectileMovementComponent->Bounciness = 0.3f;
        ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    }

    if (!BulletFX)
    {
        BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
        BulletFX->SetupAttachment(RootComponent);
    }

    if (!ProjectileSkeletalMesh)
    {
        ProjectileSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh "));
        ProjectileSkeletalMesh->SetGenerateOverlapEvents(false);
        ProjectileSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));
        ProjectileSkeletalMesh->SetupAttachment(RootComponent);
    }

    if (!ProjectileMesh)
    {
        ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));
        ProjectileMesh->SetupAttachment(RootComponent);
        ProjectileMesh->SetCanEverAffectNavigation(false);
    }
}

/////////////////////////////
////////	Basic Overloaded
////////////////////////////

// Called when the game starts or when spawned
void AArenaProjectile::BeginPlay()
{
	Super::BeginPlay();
    Execute_StartEffectLoading(this);

    ProjectileMesh->OnComponentHit.AddDynamic(this, &AArenaProjectile::OnHit);
}

// Called every frame
void AArenaProjectile::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AArenaProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor != nullptr && OtherActor != this && OtherComponent != nullptr && this->GetClass() != OtherActor->GetClass())
    {

        //TODO это костыль, надо будет переделать
        if (!(Cast<AArenaPlayerPawn>(OtherActor) && ProjectileSetting.OwnerType == EOwnerType::Player))
        {

            const TArray<AActor*> IgnoredActor;

            if (ProjectileSetting.ProjectileType == EProjectileType::Simple) {
                isPointDamage = true;
                UGameplayStatics::ApplyPointDamage(OtherActor,
                                               Damage,
                                               Hit.TraceStart,
                                               Hit,
                                               GetInstigatorController(),
                                               this,
                                               nullptr);
                isPointDamage = false;
                UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
                    1,
                    1,
                    GetActorLocation(),
                    0,
                    Execute_MaxEffectRadius(this),
                    5,
                    nullptr, IgnoredActor, this, nullptr);
            }
            else {
                isPointDamage = false;
                //DrawDebugSphere(GetWorld(), GetActorLocation(), MaxDamageDistance, 8, FColor::Red, false, 4.0f);
                //DrawDebugSphere(GetWorld(), GetActorLocation(), MinDamageDistance, 8, FColor::Blue, false, 4.0f);
            

                if (ProjectileSetting.ExploseFX)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSetting.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
                }
                if (ProjectileSetting.ExploseSound)
                {
                    UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.ExploseSound, GetActorLocation());
                }
                float rad = Execute_MaxEffectRadius(this);
                FVector vec = GetActorLocation();
                UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
                    1,
                    1,
                    vec,
                    0,
                    rad,
                    5,
                    nullptr, IgnoredActor, this, nullptr);
            }
        }

        if (ProjectileSetting.SpawnObject)
        {
            int RandVal = rand();
            if (RandVal % 100 < ProjectileSetting.ChanceToSpawn*100)
            {
                const FVector SpawnLocation = GetActorLocation();
                const FRotator SpawnRotation = FRotator::ZeroRotator;

                FActorSpawnParameters SpawnParams;
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = GetInstigator();
	
                GetWorld()->SpawnActor(ProjectileSetting.SpawnObject, &SpawnLocation, &SpawnRotation, SpawnParams);
            }
        }
    }

    Destroy();
}

/////////////////////////////
////////	Common
////////////////////////////

void AArenaProjectile::InitProjectile(const FProjectileSetting NewProjectileSetting,
                                      const FVector& ShootDirection,
                                      float InputDamage,
                                      float InputMaxDamageDistance,
                                      float InputMinDamageDistance)
{
    ProjectileSetting = NewProjectileSetting;
    Damage = InputDamage;
    MaxDamageDistance = InputMaxDamageDistance;
    MinDamageDistance = InputMinDamageDistance;
    
    InitialLifeSpan = ProjectileSetting.ProjectileLifeTime;
    ProjectileMesh = ProjectileSetting.ProjectileStaticMesh;

    BulletFX->SetTemplate(ProjectileSetting.ProjectileTrailFX);

    if (ProjectileSetting.ProjectileType == EProjectileType::Simple)
    {
        MoveToDirection(ShootDirection);
    }
    else if (ProjectileSetting.ProjectileType == EProjectileType::Rocket)
    {
        MoveToDirection(ShootDirection*FVector(1, 1, 0) + ProjectileSetting.VectorOfClimb);

        FTimerDelegate TimerDelegate_Climb;
        TimerDelegate_Climb.BindUFunction(this, FName("OutClimb"), ShootDirection*FVector(1, 1, 0));
        
        GetWorldTimerManager().SetTimer(TimerHandle_Сlimb,
                                    TimerDelegate_Climb,
                                    ProjectileSetting.TimeOfClimb,
                                    false);
    }
    
}

void AArenaProjectile::OutClimb(const FVector& ShootDirection) const
{
    MoveToDirection(ShootDirection);
}


/** set end direction for projectile */
void AArenaProjectile::MoveToDirection(const FVector& ShootDirection) const
{
    FVector Direction = ShootDirection - GetActorLocation();
    Direction.Normalize();
    
    ProjectileMovementComponent->InitialSpeed = ProjectileSetting.InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = ProjectileSetting.MaxSpeed;
    
    ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}

/////////////////////////////
////////	Effect user
////////////////////////////

void AArenaProjectile::SetMyOwner(AActor* myowner) {
    MyOwner = myowner;
}
AActor* AArenaProjectile::GetMyOwner_Implementation() {
    return MyOwner;
}


void AArenaProjectile::LoadEffect_Implementation(UEffectsMeta* effect) {
    UEffect* o = NewObject<UEffect>();
    o->Init(effect);
    Execute_ApplyEffect(this, o, 0);
}
void AArenaProjectile::LoadAllEffects_Implementation(TArray<UEffectsMeta*>& effects) {
    for (int i = 0; i < effects.Num(); i++)
        Execute_LoadEffect(this, effects[i]);
}
int AArenaProjectile::ApplyEffect_Implementation(UEffect* effect, int toIndex) {
    //EffectsToEnemy.Add(effect);
    UEffect* eff = NewObject<UEffect>();
    eff->Copy(effect);
    eff->index = toIndex ? toIndex : EffectIndex++;

    switch (eff->GetTypeOfType())
    {
    case ETepesEffectType::UP_USER:
        eff->CallStart(this, this);
        OtherEffects.Add(eff);
        break;
    case ETepesEffectType::UP_EFFECT:
        for (int i = 0; i < EffectsToEnemy.Num(); i++)
            eff->CallBuffEffect(EffectsToEnemy[i], this);
        EffectsToBuffEffects.Add(eff);
        break;
    case ETepesEffectType::INFLUENCE:
        for (int i = 0; i < EffectsToBuffEffects.Num(); i++)
            EffectsToBuffEffects[i]->CallBuffEffect(eff, this);
        EffectsToEnemy.Add(eff);
        break;
    default:
        break;
    }
    return eff->index;
}
void AArenaProjectile::ApplyAllEffects_Implementation(TArray<UEffect*>& effects, int toIndex) {
    for (int i = 0; i < effects.Num(); i++)
        Execute_ApplyEffect(this, effects[i], toIndex);
}
void AArenaProjectile::CancelEffectByIndex_Implementation(int index) {
    for (int i = OtherEffects.Num() - 1; i >= 0; i--)
        if (OtherEffects[i]->index == index) {
            OtherEffects[i]->CallCancel(this, this);
            OtherEffects.RemoveAt(i);
            return;
        }
    for (int i = EffectsToBuffEffects.Num() - 1; i >= 0; i--)
        if (EffectsToBuffEffects[i]->index == index) {
            for (int j = 0; j < EffectsToEnemy.Num(); j++)
                EffectsToBuffEffects[i]->CallDeBuffEffect(EffectsToEnemy[j], this);
            EffectsToBuffEffects[i]->CallCancel(this, this);
            EffectsToBuffEffects.RemoveAt(i);
            return;
        }
    for (int i = EffectsToEnemy.Num() - 1; i >= 0; i--)
        if (EffectsToEnemy[i]->index == index) {
            EffectsToEnemy[i]->CallCancel(this, this);
            EffectsToEnemy.RemoveAt(i);
            return;
        }
}
TArray<int> AArenaProjectile::AddEffectsToWeapon_Implementation(const TArray<UEffect*>& effects) {
    return TArray<int>();
}


TArray<UEffect*> AArenaProjectile::GetEffectsToWeapon_Implementation() {
    return TArray<UEffect*>();
}

TArray<UEffect*> AArenaProjectile::GetAllEffectsOnDistantion_Implementation(float distantion, AActor* who) {
    return FilterEffects(GetAllEffectsOnDistantion(distantion, EffectsToEnemy), who);
}
TArray<UEffect*> AArenaProjectile::GetAllEffectsOnPoint_Implementation(AActor* who) {
    return FilterEffects(GetAllEffectsOnPoint(EffectsToEnemy), who);
}
TArray<UEffect*> AArenaProjectile::GetAllEffects_Implementation(AActor* who) {
    return FilterEffects(EffectsToEnemy, who);
}
float AArenaProjectile::MaxEffectRadius_Implementation() {
    return GetMaxEffectRadius(EffectsToEnemy);
}
bool AArenaProjectile::IsPointDamage_Implementation()
{
    return isPointDamage;
}


void AArenaProjectile::StartEffectLoading_Implementation(){
    UArenaGameInstance* inst = Cast<UArenaGameInstance>(GetGameInstance());
    if (inst) {
        TArray<UEffectsMeta*> metas = inst->GetEffectsToApply(GetClass());
        Execute_LoadAllEffects(this, metas);
    }
}



/////////////////////////////
////////	Effect Influenced
////////////////////////////

void AArenaProjectile::AddAllEffects_Implementation(TArray<UEffect*>& effects, AActor* other, float radius) {
    for (int i = 0; i < effects.Num(); i++)
        Execute_AddEffect(this, effects[i], other, radius);
}
void AArenaProjectile::AddEffect_Implementation(UEffect* effect, AActor* other, float radius) {
    Execute_AddEffectComponent(this, CreateComponentFromEffect(effect, this, other, radius));
}
void AArenaProjectile::AddEffectComponent_Implementation(UEffectComponent* component) {
    component->Activate(true);
}



void AArenaProjectile::ChangeSpeed_Implementation(float speed) {}
void AArenaProjectile::ChangeHealth_Implementation(float health, AActor* other) {}
void AArenaProjectile::ChangeMaxHealth_Implementation(float health, AActor* other) {}
void AArenaProjectile::ChangeProtection_Implementation(float RateProtect) {}

void AArenaProjectile::AddWeapon_Implementation(const FString& name) {}
void AArenaProjectile::AddProjectile_Implementation(int count) {}
void AArenaProjectile::AddMagazineSize_Implementation(int count) {}

void AArenaProjectile::SetParalisys_Implementation(bool isParalisys) {}

bool AArenaProjectile::IsParalisys_Implementation() {
    return false;
}