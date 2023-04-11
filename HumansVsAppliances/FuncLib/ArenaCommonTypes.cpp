// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaCommonTypes.h"
#include "HumansVsAppliances/Core/ArenaGameInstance.h"
#include "HumansVsAppliances/EnemiesPawn/EnemyPawn.h"
#include "HumansVsAppliances/Weapons/ArenaWeapon.h"



void UArenaCommonTypes::RepulseEnemy(AEnemyPawn* EnemyPawn, FVector DirToRepulsion)
{
	const FVector FrdVec = EnemyPawn->GetActorForwardVector();
	EnemyPawn->LaunchCharacter(EnemyPawn->GetVelocity() + DirToRepulsion,true, true);
}

AArenaWeapon* UArenaCommonTypes::SetWeapon(UWorld* CurUWorld,
										   USkeletalMeshComponent* TargetMesh,
										   AActor* CurOwner,
										   APawn* CurInstigator,
										   APlayerController* CurPlayerController,
										   const FName NameWeapon)
{
	UArenaGameInstance* MyGI = Cast<UArenaGameInstance>(CurUWorld->GetGameInstance());
	if (MyGI)
	{
		FWeaponInfo MyInfo;
		if (MyGI->GetWeaponInfoByName(NameWeapon, MyInfo))
		{
			const FVector CurSpawnLocation = FVector::ZeroVector;
			const FRotator CurSpawnRotation = FRotator::ZeroRotator;

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = CurOwner;
			SpawnParams.Instigator = CurInstigator;
			
			AArenaWeapon* NewWeapon = Cast<AArenaWeapon>(CurUWorld->SpawnActor(MyInfo.WeaponClass,
																		&CurSpawnLocation,
																		&CurSpawnRotation,
																		SpawnParams));
			if (NewWeapon)
			{
				const FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
				NewWeapon->AttachToComponent(TargetMesh, Rule, MyInfo.NameAttachSocket);
				
				NewWeapon->NameWeapon = NameWeapon;
				NewWeapon->CurrentWeaponInfo = MyInfo;

				NewWeapon->SetCurPlayerController(CurPlayerController);

				NewWeapon->CurrentWeaponInfo.Damage *= MyInfo.DamageFactor;

				NewWeapon->SetMyOwner(CurOwner);

				return NewWeapon;
			}
		}
	}
	return nullptr;
}
