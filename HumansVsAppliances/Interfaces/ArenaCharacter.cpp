// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaCharacter.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HumansVsAppliances/ActorComponents/ArenaHealthActorComponent.h"
#include "../Core/ArenaGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AArenaCharacter::AArenaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UArenaHealthActorComponent>(TEXT("HealthComponent"));
	if (HealthComponent)
	{
		HealthComponent->OnHealthEmpty.AddDynamic(this, &AArenaCharacter::CharacterDeadAnim);
	}
}

// Called when the game starts or when spawned
void AArenaCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AArenaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AArenaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AArenaCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsJumping)
	{
		bool attack = false;
		if (Cast<UArenaGameInstance>(GetGameInstance())->IsEnvironment(OtherActor)) {

			bIsJumping = false;
			attack = true;
			//DrawDebugSphere(GetWorld(), GetActorLocation(), MaxJumpDamageDistance, 8, FColor::Red, false, 4.0f);
			//DrawDebugSphere(GetWorld(), GetActorLocation(), MinJumpDamageDistance, 8, FColor::Blue, false, 4.0f);
		}
		
		if (bReadyJumpingDamage) {
			if (abs(FVector::DotProduct(Hit.Normal, FVector::UpVector)) > 0.5) {
				attack = true;
				UArenaGameInstance* inst = Cast< UArenaGameInstance>(GetGameInstance());
				if (inst)
					inst->DebugString("JumpNormaled!");
			}
			else {
				UArenaGameInstance* inst = Cast< UArenaGameInstance>(GetGameInstance());
				if (inst) {
					inst->DebugString(FString::SanitizeFloat(abs(FVector::DotProduct(Hit.Normal, FVector::UpVector))));
					//inst->DebugString(FString::SanitizeFloat(GetCharacterMovement()->Velocity.Z));
				}
			}
			if (attack) {
				bReadyJumpingDamage = false;
				TArray<AActor*> IgnoredActor;
				IgnoredActor.Add(this);
				UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
					1000,
					1000 * 0.2f,
					GetActorLocation(),
					0,
					GetJumpingDamageRadius(),
					5,
					NULL, IgnoredActor, GetJumpingWeapon(), nullptr);
			}
		}
	}
}
AActor* AArenaCharacter::GetJumpingWeapon(){
	return nullptr;
}
float AArenaCharacter::GetJumpingDamageRadius(){
	return 0;
}
void AArenaCharacter::CharacterDeadAnim()
{
	GetCharacterMovement()->MaxWalkSpeed = 0;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
	if (bIsAlive && AnimDead)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PlayingDeathAnim,
										this,
										&AArenaCharacter::CharacterDead,
										PlayAnimMontage(AnimDead),
										false);
	}
	else if (bIsAlive)
		CharacterDead();
	bIsAlive = false;
}

void AArenaCharacter::CharacterDead()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_PlayingDeathAnim);
	OnDead.Broadcast();
}
void AArenaCharacter::StartTakeDamageAnimation() {
	if (bIsAlive)
	{
		PlayDamageSound_BP();
		PlayAnimMontage(AnimTakeDamage);
	}
}

float AArenaCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	
		
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AArenaCharacter::PlayDamageSound_BP_Implementation()
{
	// in BP
}
