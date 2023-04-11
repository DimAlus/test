// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaPlayerController.h"
#include "ArenaGameInstance.h"
#include "ArenaGameModeBase.h"
#include "../ActorComponents/ArenaHealthActorComponent.h"
#include "../EnemiesPawn/EnemyPawn.h"
#include "../PlayerPawn/ArenaPlayerPawn.h"
#include "../Projectiles/ArenaProjectile.h"
#include "HumansVsAppliances/EnemiesPawn/ArenaEnemyAIController.h"
#include "HumansVsAppliances/PreSpawner/PreSpawner.h"
#include "HumansVsAppliances/IdleEffectUser/IdleEffectUser.h"
#include "HumansVsAppliances/Weapons/ArenaWeapon.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"


FVector GetRandomLocationOnBorder(FVector lu, FVector rd) {
	FVector res = rd - lu;
	if (res.X < 0) res.X *= -1;
	if (res.Y < 0) res.Y *= -1;
	float len = res.X * 2 + res.Y * 2;
	float r = +FMath::RandRange(0, int(len));// / RAND_MAX;	

	for (int i = 0; i < 4; i++) {
		float mns = i % 2 ? res.Y : res.X;
		if (r <= mns) {
			r /= mns;
			FVector re = lu +
				(rd - lu) * r * (i % 2 ? FVector::ForwardVector : FVector::RightVector);
			return i < 2 ? re : (re + (rd - lu) * (i % 2 ? FVector::RightVector : FVector::ForwardVector));
		}
		else r -= mns;
	}
	throw - 1;
}

/////////////////////////////
////////	Basic Overloaded
////////////////////////////

void AArenaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	

}

void AArenaPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	//SpawnEnemyActor();
}

void AArenaPlayerController::OnPossess(APawn* CurPawn)
{
	Super::OnPossess(CurPawn);
	if (AArenaPlayerPawn* MyCharacter = Cast<AArenaPlayerPawn>(GetPawn()))
	{
		CurrentPlayerCharacter = MyCharacter;
		CurrentPlayerCharacter->OnDead.AddDynamic(this, &AArenaPlayerController::CharacterDead);

		TSubclassOf<AArenaEnemyAIController> classToFind;
		classToFind = AArenaEnemyAIController::StaticClass();
		TArray<AActor*> foundAI;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundAI);
		for (auto one: foundAI)
		{
			AArenaEnemyAIController* OneController = Cast<AArenaEnemyAIController>(one);
			if (OneController)
			{
				OneController->RetargetToPlayerPawn_BP(CurrentPlayerCharacter);
			}
		}
		
	}
}

/////////////////////////////
////////	Health
////////////////////////////

void AArenaPlayerController::CharacterDead()
{
	CurrentPlayerCharacter->bIsAlive = false;
	CurrentPlayerCharacter->CurrentWeapon->Destroy();
	
	UnPossess();
	CurrentPlayerCharacter->Destroy();
	
	if (CurrentPlayerCharacter && CountPlayerLives > 1)
	{
		RespawnPlayerCharacter();
	}
	else
	{
		OnEndGame.Broadcast();
	}
	CountPlayerLives -= 1;
	OnChangeCountLives.Broadcast(CountPlayerLives);
}

/////////////////////////////
////////	Respawn Player
////////////////////////////

void  AArenaPlayerController::RespawnPlayerCharacterImpl(TSubclassOf<AActor> ActorClass, FVector locaion) {
	
}
void AArenaPlayerController::RespawnPlayerCharacter()
{
	if (const AArenaGameModeBase* MyGameMode = Cast<AArenaGameModeBase>(GetWorld()->GetAuthGameMode()))
	{
		if (MyGameMode->RespawnCharacter(this))
		{
			OnRespawnPlayerCharacter.Broadcast();
		}
	}
	/*FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	const FVector SpawnLoc = FVector::ZeroVector;
	const FRotator SpawnRot = FRotator::ZeroRotator;
	APreSpawner* spawner = Cast<APreSpawner>(GetWorld()->SpawnActor(SpawnerClass, &SpawnLoc, &SpawnRot, SpawnParams));
	if (spawner) {
		spawner->OnSpawnActorDelegate.AddDynamic(this, &AArenaPlayerController::RespawnPlayerCharacterImpl);
		spawner->Init(this, nullptr, 3);
	}*/
}

void AArenaPlayerController::SetSpawnPlatform()
{
	if (CurLevelInfo)
	{
		int StartStr = GetWorld()->GetMapName().Find("Level");
		FString MapName = GetWorld()->GetMapName().Mid(StartStr, GetWorld()->GetMapName().Len() - StartStr);
		FLevelInfo* Result = CurLevelInfo->FindRow<FLevelInfo>(*MapName, *MapName, true);
		BeginRespawnCountEnemies = Result->BeginRespawnCountEnemies;
		IncRespawnCountEnemies = Result->IncRespawnCountEnemies;
		TimeToRespawnEnemies = Result->TimeToRespawnEnemies;
		ChanceSpawnRouter = Result->ChanceSpawnRouter;
		ChanceSpawnRoboCleaner = Result->ChanceSpawnRoboCleaner;
		ChanceSpawnWashingMachine= Result->ChanceSpawnWashingMachine;
	}

	
	TArray<AActor*> FoundPlatform;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ClassSurfacePlatform, FoundPlatform);
	if (FoundPlatform.Num() != 0)
	{
		SurfacePlatform = FoundPlatform[0];
	}
	SpawnEnemyActor();

	if (bIsStartGame)
	{
		bIsStartGame = false;
		GetWorldTimerManager().SetTimer(TimerHandle_Respawn,
									this,
									&AArenaPlayerController::SpawnEnemyActor,
									TimeToRespawnEnemies,
									true);
	}
}

int AArenaPlayerController::GetRandRange(const int Min, const int Max)
{
	const int Range = Max - Min + 1;
	const int Num = rand() % Range + Min;
	return Num;
}

/////////////////////////////
////////	Score
////////////////////////////

void AArenaPlayerController::NotifyFromEnemy(EEnemyType CurType)
{
	
	switch (CurType) {
		case EEnemyType::Router:
			CountKilledRouters+=1;
			break;
		case EEnemyType::Washer:
			CountKilledWashers+=1;
			break;
		case EEnemyType::RobotVacuum:
			CountKilledRoboWacuums+=1;
			break;
		default: ;
	}

	TotalKills = CountKilledRouters+CountKilledWashers+CountKilledRoboWacuums;
	
	OnChangeTotalScore.Broadcast(TotalKills);
}

/////////////////////////////
////////	Respawn Emeny
////////////////////////////

void AArenaPlayerController::SpawnDirection() {
	// Spawn direction to camera for enemy spawn on border
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	const FVector SpawnLoc = FVector::ZeroVector;
	const FRotator SpawnRot = FRotator::ZeroRotator;
	direction = GetWorld()->SpawnActor(directionClass, &SpawnLoc, &SpawnRot, SpawnParams);
	direction->SetActorRelativeLocation(FVector(1000, 0, 0));

	const FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, false);
	//CurrentPlayerCharacter->FollowCamera

	direction->AttachToComponent(CurrentPlayerCharacter->FollowCamera, rules, FName("CAMERA_DIRECTION"));
}

void AArenaPlayerController::SpawnEnemyActorImpl(TSubclassOf<AActor> ActorClass, FVector locaion) {
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	const FVector SpawnLoc = locaion+FVector(0,0,120);
	const FRotator SpawnRot = FRotator::ZeroRotator;

	FString EnemyName;
	AEnemyPawn* NewAIChar = Cast<AEnemyPawn>(GetWorld()->SpawnActor(ActorClass, &SpawnLoc, &SpawnRot, SpawnParams));
	if (NewAIChar)
	{
		NewAIChar->OnDestroyed.AddDynamic(this, &AArenaPlayerController::EnemyDeadEvent);
		switch (NewAIChar->CutEnemyType) {
		case EEnemyType::Router:
		{
			EnemyName = "Router";
			break;
		}
		case EEnemyType::Washer:
		{
			EnemyName = "WashingMachine";
			break;
		}
		case EEnemyType::RobotVacuum:
		{
			EnemyName = "RoboCleaner";
			break;
		}
		default:
			break;
		}


		if (CurEnemiesInfo)
		{
			FEnemiesInfo* Result = CurEnemiesInfo->FindRow<FEnemiesInfo>(*EnemyName, *EnemyName, true);
			NewAIChar->SetParams(Result->HealthInPercent, Result->MaxSpeed);
		}

	}
}
void AArenaPlayerController::SpawnEnemyActor()
{

	const FVector SurfaceLoc = SurfacePlatform->GetActorLocation();
	const FVector Scale = SurfacePlatform->GetActorScale();

	const int Y_Min = 0;
	const int Y_Max = SurfaceLoc.Y + Scale.Y * SurfaceYfactor;

	const int X_Min = 0;
	const int X_Max = SurfaceLoc.X + Scale.X * SurfaceXfactor;

	const int Z_Num = SurfaceLoc.Z;


	if (!direction) SpawnDirection();
	FIntVector viewport;
	GetViewportSize(viewport.X, viewport.Y);
	direction->SetActorRelativeLocation(FVector(1000, -viewport.X, -viewport.Y));
	FVector lu = direction->GetActorLocation();
	direction->SetActorRelativeLocation(FVector(1000, viewport.X, viewport.Y));
	FVector rd = direction->GetActorLocation();
	direction->SetActorRelativeLocation(FVector(0, 0, 0));
	FVector ts = direction->GetActorLocation();


	float scaler = (SurfaceLoc.Z - ts.Z) / (lu.Z - ts.Z);
	lu = ts + (lu - ts) * scaler;
	scaler = (SurfaceLoc.Z - ts.Z) / (rd.Z - ts.Z);
	rd = ts + (rd - ts) * scaler;


	for (int i = 0; i < BeginRespawnCountEnemies; ++i)
	{
		if (CurrentPlayerCharacter && SurfacePlatform)
		{
			/*FVector pos;
			int cntLoops = 0;
			while (1) {
				if (cntLoops++ > 40) {
					lu = FVector(X_Min, Y_Min, 0);
					rd = FVector(X_Max, Y_Max, 0);
				}
				pos = GetRandomLocationOnBorder(lu, rd);
				if (pos.X <= X_Max &&
					pos.X >= X_Min &&
					pos.Y <= Y_Max &&
					pos.Y >= Y_Min)
					break;
			}
			const int X_Num = pos.X;//GetRandRange(X_Min, X_Max);
			const int Y_Num = pos.Y;//GetRandRange(Y_Min, Y_Max);*/
			const int X_Num = GetRandRange(X_Min, X_Max);
			const int Y_Num = GetRandRange(Y_Min, Y_Max);


			const FVector SpawnLoc = FVector(X_Num/* + FMath::RandRange(-200, 200)*/, Y_Num /*+ FMath::RandRange(-200, 200)*/, Z_Num);


			int RandVal = FMath::RandRange(0, 100);
			TSubclassOf<class AEnemyPawn> SpawnClassEnemies;
			if (RandVal <= ChanceSpawnRouter)
				SpawnClassEnemies = ClassEnemiesRouter;
			else if (RandVal > ChanceSpawnRouter && RandVal <= ChanceSpawnRouter + ChanceSpawnRoboCleaner)
				SpawnClassEnemies = ClassEnemiesRoboCleaner;
			else
				SpawnClassEnemies = ClassEnemiesWashingMachine;

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			//const FVector SpawnLoc = FVector::ZeroVector;
			const FRotator SpawnRot = FRotator::ZeroRotator;
			APreSpawner* spawner = Cast<APreSpawner>(GetWorld()->SpawnActor(SpawnerClass, &SpawnLoc, &SpawnRot, SpawnParams));
			if (spawner) {
				spawner->OnSpawnActorDelegate.AddDynamic(this, &AArenaPlayerController::SpawnEnemyActorImpl);
				spawner->Init(this, SpawnClassEnemies, 1);

			}
			//SpawnEnemyActorImpl(SpawnClassEnemies, SpawnLoc);
		}
	}
	BeginRespawnCountEnemies += IncRespawnCountEnemies;
}
void AArenaPlayerController::EnemyDeadEvent(AActor* actor){
	if (sumdrops < 0) {
		sumdrops = 0;
		for (int i = 0; i < drops.Num(); i++) {
			sumdrops += drops[i].chance;
			drops[i].limit = sumdrops;
		}
	}
	AEnemyPawn* enemy = Cast<AEnemyPawn>(actor);
	if (enemy) {
		float ch = sumdrops * rand() / RAND_MAX;
		for (int i = 0; i < drops.Num(); i++)
			if (drops[i].limit > ch) {
				int ZHeight = 0;
				switch (enemy->CutEnemyType) {
					case EEnemyType::Router:
						ZHeight = -70;
						break;
					case EEnemyType::Washer:
						ZHeight = -70;
						break;
					case EEnemyType::RobotVacuum:
						ZHeight = -50;
						break;
				default: ;
				}

				SpawnDrop(drops[i].DropClass, enemy->GetActorLocation() + FVector(0, 0, ZHeight));
				break;
			}
	}
}
void AArenaPlayerController::SpawnDrop(TSubclassOf<AIdleEffectUser> DropClass, FVector Aim){
	if (DropClass) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		const FRotator SpawnRot = FRotator::ZeroRotator;

		AIdleEffectUser* dropObject = Cast<AIdleEffectUser>(GetWorld()->SpawnActor(DropClass, &Aim, &SpawnRot, SpawnParams));
		if (dropObject) {
			TArray<FEnemyDropData> mydrops = GetDropRandEffects(DropClass);
			dropObject->InitWithEffects(mydrops);
		}

	}
}
TArray<FEnemyDropData> AArenaPlayerController::GetDropRandEffects(TSubclassOf<AIdleEffectUser> DropClass)
{
	/*TArray<FEnemyDropData> res;
	for (int i = 0; i < dropRandomizer.Num(); i++) {
		if (dropRandomizer[i].DropClass == DropClass) {
			if (dropRandomizer[i].sumchances < 0) {
				float schs = 0;
				for (int j = 0; j < dropRandomizer[i].OneFrom.Num(); j++)
					schs += dropRandomizer[i].OneFrom[j].chance;
				dropRandomizer[i].sumchances = schs;
			}

			float ch = dropRandomizer[i].sumchances * rand() / RAND_MAX;
			float summ = 0;
			for (int j = 0; j < dropRandomizer[i].OneFrom.Num(); j++)
				if ((summ += dropRandomizer[i].OneFrom[j].chance) > ch) {
					res.Add(dropRandomizer[i].OneFrom[j]);
					break;
				}
		}

	}*/
	UArenaGameInstance* inst = Cast<UArenaGameInstance>(GetGameInstance());
	if (inst)
		return inst->GetDropEffectsToPlayer(DropClass);
	return TArray<FEnemyDropData>();
}

TArray<FString> AArenaPlayerController::GetCSVFile(FString Path)
{
	TArray<FString> Lines;
	FString CsvFile = /*FPaths::ProjectContentDir() +*/ Path;
	if (FPaths::FileExists(CsvFile))
	{
		UE_LOG(LogTemp, Log, TEXT("File %s exists"), *CsvFile);		
		FFileHelper::LoadANSITextFileToStrings(*CsvFile, NULL, Lines);
	}

	return Lines;
}

UDataTable*  AArenaPlayerController::LoadCSVDataFile(ECSVTableType CurTableType)
{
	FString Path = FPaths::ProjectDir();
	Path.Append("Tables");
	UClass* DataTableClass = UDataTable::StaticClass(); // get a reference to the type of object we are going to use, in this case the basic DataTable, but you can have your own
	UDataTable* SelectedClassProgressionDataTable = nullptr;

	UArenaGameInstance* CurGameInst = Cast<UArenaGameInstance>(GetGameInstance());
	if (CurGameInst)
	{
		switch (CurGameInst->CurDiffLevel) {
		case EDifficultyLevel::Simple:
			Path = Path + "/Simple";
			break;
		case EDifficultyLevel::Middle:
			Path = Path + "/Middle";
			break;
		case EDifficultyLevel::Hard:
			Path = Path + "/Hard";
			break;
		default: ;
		}
	}
	else
	{
		return SelectedClassProgressionDataTable;
	}
	
	switch (CurTableType) {
		case ECSVTableType::Levels:
		{
			SelectedClassProgressionDataTable = NewObject<UDataTable>(this, DataTableClass, FName(TEXT("Levels")));
			SelectedClassProgressionDataTable->RowStruct = FLevelInfo::StaticStruct();
			Path += "/Levels.csv";
			break;
		}
		case ECSVTableType::Enemies:
		{
			SelectedClassProgressionDataTable = NewObject<UDataTable>(this, DataTableClass, FName(TEXT("Enemies")));
			SelectedClassProgressionDataTable->RowStruct = FEnemiesInfo::StaticStruct();
			Path += "/Enemies.csv";
			break;
		}
		default: ;
	}
	
	
	TArray<FString> CSVLines = GetCSVFile(Path);
	
	
	for (int i = 1; i < CSVLines.Num(); i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> stringArray = {};
		aString.ParseIntoArray(stringArray, TEXT(";"), false);
		if (stringArray.Num() == 0){continue;}
		FString LineLabel = stringArray[0];
		if ((LineLabel.Len() == 0)  || LineLabel.StartsWith("\";") || LineLabel.StartsWith(";"))
		{
			continue;
		}

		switch (CurTableType) {
			case ECSVTableType::Levels:
			{
				FLevelInfo Row;
				Row.LevelTimeSec = FCString::Atoi(*stringArray[1]);
				Row.BeginRespawnCountEnemies = FCString::Atoi(*stringArray[2]);
				Row.IncRespawnCountEnemies = FCString::Atoi(*stringArray[3]);
				Row.TimeToRespawnEnemies = FCString::Atof(*stringArray[4]);
				Row.ChanceSpawnRouter = FCString::Atof(*stringArray[5]);
				Row.ChanceSpawnRoboCleaner = FCString::Atof(*stringArray[6]);
				Row.ChanceSpawnWashingMachine = FCString::Atof(*stringArray[7]);
				SelectedClassProgressionDataTable->AddRow(FName(*stringArray[0]), Row);	
				break;
			}
			case ECSVTableType::Enemies:
			{
				FEnemiesInfo Row;
				Row.HealthInPercent = FCString::Atoi(*stringArray[1]);
				Row.MaxSpeed = FCString::Atoi(*stringArray[2]);
				SelectedClassProgressionDataTable->AddRow(FName(*stringArray[0]), Row);
				break;
			}
			default: ;
		}
	}
	
	return SelectedClassProgressionDataTable;
}

void  AArenaPlayerController::LoadRow(UDataTable* CurDataTable, FString RowName, FLevelInfo& OutInfo)
{
	FLevelInfo* Result = CurDataTable->FindRow<FLevelInfo>(*RowName, *RowName, true);
	OutInfo = *Result;
}
